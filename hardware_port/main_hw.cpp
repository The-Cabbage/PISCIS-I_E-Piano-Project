// main_hw.cpp - 硬件实时音频 + 串口 MIDI 输入 (直接使用 mdaEPiano)
#include <alsa/asoundlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include "mdaEPiano.h"
#include "config_hw.h"

// ==================== 全局变量定义（供 mdaEPiano.cpp 链接） ====================
config_t configuration = {0};
uint8_t master_volume = 80;
void set_master_volume(uint8_t value) {
    // 空实现，可扩展
}

// ==================== 硬件参数 ====================
static const unsigned int SAMPLE_RATE_HZ = AUDIO_SAMPLE_RATE;
static const snd_pcm_uframes_t PERIOD_SIZE = AUDIO_BLOCK_SAMPLES;
static const unsigned int CHANNELS = 2;

// 串口配置（根据实际修改）
static const char* UART_DEVICE = "/dev/ttyS0";
static const speed_t UART_BAUD = B115200;

static volatile bool g_running = true;

// ==================== 信号处理 ====================
void signal_handler(int sig) {
    (void)sig;
    g_running = false;
}

// ==================== MIDI 运行状态解析器 ====================
class MidiParser {
public:
    MidiParser() : m_running_status(0), m_data_index(0) {}

    bool parse(uint8_t byte, int* out_status, int* out_data1, int* out_data2) {
        if (byte & 0x80) {
            m_running_status = byte;
            m_data_index = 0;
            if ((byte & 0xF0) == 0xF0) return false; // 系统消息暂不处理
            return false;
        } else {
            if (m_running_status == 0) return false;
            m_data[m_data_index++] = byte;
            int msg_len = 0;
            int status_high = m_running_status & 0xF0;
            if (status_high == 0x80 || status_high == 0x90 ||
                status_high == 0xA0 || status_high == 0xB0 ||
                status_high == 0xE0) {
                msg_len = 2;
            } else if (status_high == 0xC0 || status_high == 0xD0) {
                msg_len = 1;
            } else {
                m_data_index = 0;
                return false;
            }
            if (m_data_index == msg_len) {
                *out_status = m_running_status;
                *out_data1 = m_data[0];
                *out_data2 = (msg_len == 2) ? m_data[1] : 0;
                m_data_index = 0;
                // Note On 且 velocity 0 转为 Note Off
                if ((*out_status & 0xF0) == 0x90 && *out_data2 == 0) {
                    *out_status = 0x80 | (*out_status & 0x0F);
                }
                return true;
            }
            return false;
        }
    }

private:
    uint8_t m_running_status;
    uint8_t m_data[2];
    int m_data_index;
};

// ==================== 主函数 ====================
int main() {
    int err;
    snd_pcm_t *pcm_handle = nullptr;
    snd_pcm_hw_params_t *hw_params = nullptr;
    int16_t *interleaved_buffer = nullptr;
    size_t buffer_bytes = PERIOD_SIZE * CHANNELS * sizeof(int16_t);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // ---------- 1. 初始化 MDA 引擎 ----------
    auto engine = std::make_unique<mdaEPiano>();
    engine->setVolume(0.8f);

    // ---------- 2. 打开 ALSA 设备 ----------
    err = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        std::cerr << "❌ 无法打开 ALSA 设备 'default': " << snd_strerror(err) << std::endl;
        return EXIT_FAILURE;
    }

    // ---------- 3. 设置硬件参数 ----------
    snd_pcm_hw_params_malloc(&hw_params);
    if (!hw_params) {
        std::cerr << "❌ 内存分配失败" << std::endl;
        snd_pcm_close(pcm_handle);
        return EXIT_FAILURE;
    }

    err = snd_pcm_hw_params_any(pcm_handle, hw_params);
    if (err < 0) goto cleanup;

    err = snd_pcm_hw_params_set_access(pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) goto cleanup;

    err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    if (err < 0) goto cleanup;

    err = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, CHANNELS);
    if (err < 0) goto cleanup;

    unsigned int actual_rate = SAMPLE_RATE_HZ;
    err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &actual_rate, 0);
    if (err < 0) goto cleanup;

    snd_pcm_uframes_t period_frames = PERIOD_SIZE;
    err = snd_pcm_hw_params_set_period_size_near(pcm_handle, hw_params, &period_frames, 0);
    if (err < 0) goto cleanup;

    err = snd_pcm_hw_params(pcm_handle, hw_params);
    if (err < 0) goto cleanup;

    snd_pcm_hw_params_free(hw_params);
    hw_params = nullptr;

    // ---------- 4. 分配音频缓冲区 ----------
    interleaved_buffer = (int16_t*)malloc(buffer_bytes);
    if (!interleaved_buffer) {
        std::cerr << "❌ 缓冲区内存分配失败" << std::endl;
        goto cleanup;
    }

    // ---------- 5. 打开串口 ----------
    int uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd < 0) {
        std::cerr << "❌ 无法打开串口 " << UART_DEVICE << ": " << strerror(errno) << std::endl;
        goto cleanup;
    }

    struct termios tty;
    if (tcgetattr(uart_fd, &tty) != 0) {
        std::cerr << "❌ tcgetattr 失败: " << strerror(errno) << std::endl;
        close(uart_fd);
        goto cleanup;
    }

    cfsetospeed(&tty, UART_BAUD);
    cfsetispeed(&tty, UART_BAUD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0) {
        std::cerr << "❌ tcsetattr 失败: " << strerror(errno) << std::endl;
        close(uart_fd);
        goto cleanup;
    }

    std::cout << "✅ 串口已打开，等待 MIDI 输入..." << std::endl;

    // ---------- 6. MIDI 解析器 ----------
    MidiParser parser;

    // ---------- 7. 主循环 ----------
    int16_t bufferL[PERIOD_SIZE];
    int16_t bufferR[PERIOD_SIZE];
    fd_set read_fds;
    struct timeval tv;

    while (g_running) {
        FD_ZERO(&read_fds);
        FD_SET(uart_fd, &read_fds);
        tv.tv_sec = 0;
        tv.tv_usec = 1000;

        int ret = select(uart_fd + 1, &read_fds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(uart_fd, &read_fds)) {
            uint8_t byte;
            ssize_t n = read(uart_fd, &byte, 1);
            if (n == 1) {
                int status, data1, data2;
                if (parser.parse(byte, &status, &data1, &data2)) {
                    int cmd = status & 0xF0;
                    switch (cmd) {
                        case 0x80: engine->noteOn(data1, 0); break;
                        case 0x90: engine->noteOn(data1, data2); break;
                        default: break;
                    }
                }
            }
        }

        // 渲染音频
        engine->process(bufferR, bufferL);  // 参数顺序：右，左

        // 交错并写入 ALSA
        for (size_t i = 0; i < PERIOD_SIZE; i++) {
            interleaved_buffer[2*i] = bufferL[i];
            interleaved_buffer[2*i + 1] = bufferR[i];
        }

        err = snd_pcm_writei(pcm_handle, interleaved_buffer, PERIOD_SIZE);
        if (err == -EPIPE) {
            snd_pcm_prepare(pcm_handle);
        } else if (err < 0) {
            std::cerr << "❌ 写入音频失败: " << snd_strerror(err) << std::endl;
            break;
        }
    }

    std::cout << "🛑 退出..." << std::endl;

cleanup:
    if (interleaved_buffer) free(interleaved_buffer);
    if (hw_params) snd_pcm_hw_params_free(hw_params);
    if (pcm_handle) {
        snd_pcm_drain(pcm_handle);
        snd_pcm_close(pcm_handle);
    }
    if (uart_fd >= 0) close(uart_fd);

    return (err < 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}