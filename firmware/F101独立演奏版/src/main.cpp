#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <errno.h>
#include <string.h>
#include "DualSynth.h"
#include "MidiInput.h"

extern "C" {
#include "midi_host.h"
}

namespace {
constexpr unsigned frames = 256;
// Leave two queued blocks after the driver fills its four DMA periods.
constexpr unsigned startup_blocks = 6;
constexpr size_t block_bytes = frames * 2 * sizeof(int16_t);
constexpr uint8_t codec_address = 0x10; // 7-bit I2C address with CE grounded
constexpr uint8_t dac_control3 = 0x19;
constexpr uint8_t dac_mute = 0x04;

const struct device *const i2c = DEVICE_DT_GET(DT_NODELABEL(i2c0));
const struct device *const i2s = DEVICE_DT_GET(DT_NODELABEL(i2s0));
float left[frames], right[frames];
K_MEM_SLAB_DEFINE(audio_blocks, block_bytes, startup_blocks, 64);
K_MSGQ_DEFINE(midi_events, sizeof(struct midi_event), 128, 4);
K_THREAD_STACK_DEFINE(midi_stack, 4096);
struct k_thread midi_thread;

class PlaybackVolume {
    static constexpr float max_gain = 8.0f;
    // Leave approximately 3 dB of digital peak headroom for the analog gain.
    static constexpr float ceiling = 0.70f;
    float gain = max_gain;
    float target = max_gain;
    float limiter_gain = 1.0f;
    float pending_peak = 0.0f;
    float pending_left[frames]{};
    float pending_right[frames]{};

    static float magnitude(float sample)
    {
        return sample < 0 ? -sample : sample;
    }

public:
    void set(uint8_t value)
    {
        // A squared taper gives finer control near the quiet end of the knob.
        const float position = value / 127.0f;
        target = max_gain * position * position;
    }

    void interleave(int16_t *pcm)
    {
        float peak = 0.0f;
        const float step = (target - gain) / frames;
        for (unsigned i = 0; i < frames; ++i) {
            const float level = gain + step * (i + 1);
            left[i] *= level;
            right[i] *= level;
            const float l = magnitude(left[i]);
            const float r = magnitude(right[i]);
            if (l > peak) peak = l;
            if (r > peak) peak = r;
        }
        gain = target;

        // Delay one block. This lets the gain ramp down before a loud chord
        // arrives, while a slower release keeps it from following the waveform.
        const float largest = peak > pending_peak ? peak : pending_peak;
        const float safe_gain = largest > ceiling ? ceiling / largest : 1.0f;
        const float recovery = limiter_gain + (1.0f - limiter_gain) * 0.04f;
        const float next_gain = recovery < safe_gain ? recovery : safe_gain;
        const float limiter_step = (next_gain - limiter_gain) / frames;
        for (unsigned i = 0; i < frames; ++i) {
            const float level = (limiter_gain + limiter_step * (i + 1)) * 32767.0f;
            pcm[2 * i] = static_cast<int16_t>(pending_left[i] * level);
            pcm[2 * i + 1] = static_cast<int16_t>(pending_right[i] * level);
            pending_left[i] = left[i];
            pending_right[i] = right[i];
        }
        limiter_gain = next_gain;
        pending_peak = peak;
    }
};

PlaybackVolume playback_volume;

int codec_write(uint8_t reg, uint8_t value)
{
    return i2c_reg_write_byte(i2c, codec_address, reg, value);
}

int codec_prepare()
{
    uint8_t reg_value = 0;
    int ret = i2c_reg_read_byte(i2c, codec_address, 0x00, &reg_value);
    if (ret) {
        printk("ES8388 did not respond at 0x10: %d\n", ret);
        return ret;
    }

    // Playback path from the ES8388 register map. CE is strapped low on the
    // PISCIS carrier and J5 is fed by LOUT1/ROUT1.
    constexpr struct { uint8_t reg, value; } setup[] = {
        {dac_control3, dac_mute},
        {0x01, 0x50}, // Reference and analog power
        {0x02, 0x00}, // Chip normal operation
        {0x03, 0xff}, // ADC off
        {0x04, 0xc0}, // DAC and analog outputs off during configuration
        {0x00, 0x12}, // Playback reference configuration
        {0x08, 0x00}, // F101 supplies clocks
        {0x17, 0x18}, // Philips I2S, 16-bit samples
        {0x18, 0x02}, // MCLK = 256 x sample rate
        {0x1a, 0x00}, {0x1b, 0x00}, // Digital playback volume (0 dB)
        {0x26, 0x00},
        {0x27, 0x90}, {0x2a, 0x90}, // Left/right DAC to matching analog mixer
        {0x2b, 0x80}, // Shared LRCK
        {0x2d, 0x00},
        {0x2e, 0x21}, {0x2f, 0x21}, // LOUT1/ROUT1 analog gain +3 dB
        {0x04, 0x30}, // Enable DAC and the LOUT1/ROUT1 pair
    };
    for (const auto &entry : setup) {
        ret = codec_write(entry.reg, entry.value);
        if (ret) {
            printk("ES8388 register 0x%02x failed: %d\n", entry.reg, ret);
            return ret;
        }
    }
    k_msleep(100);
    return 0;
}

int queue_audio(DualSynth *synth)
{
    void *block;
    int ret = k_mem_slab_alloc(&audio_blocks, &block, K_MSEC(100));
    if (ret) return ret;
    if (synth) {
        synth->render(left, right, frames);
        playback_volume.interleave(static_cast<int16_t *>(block));
    } else {
        memset(block, 0, block_bytes);
    }
    ret = i2s_write(i2s, block, block_bytes);
    if (ret) k_mem_slab_free(&audio_blocks, block);
    return ret;
}

int audio_start()
{
    if (!device_is_ready(i2c) || !device_is_ready(i2s)) return -ENODEV;
    int ret = codec_prepare();
    if (ret) return ret;
    const struct i2s_config cfg = {
        .word_size = 16,
        .channels = 2,
        .format = I2S_FMT_DATA_FORMAT_I2S,
        .options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER,
        .frame_clk_freq = 44100,
        .mem_slab = &audio_blocks,
        .block_size = block_bytes,
        .timeout = 100,
    };
    ret = i2s_configure(i2s, I2S_DIR_TX, &cfg);
    if (ret) return ret;
    for (unsigned i = 0; i < startup_blocks && !ret; ++i) ret = queue_audio(nullptr);
    if (!ret) ret = i2s_trigger(i2s, I2S_DIR_TX, I2S_TRIGGER_START);
    if (!ret) ret = codec_write(dac_control3, 0x00);
    if (ret) i2s_trigger(i2s, I2S_DIR_TX, I2S_TRIGGER_DROP);
    return ret;
}

void midi_receiver(void *, void *, void *)
{
    const int ret = midi_host_start();
    if (ret) {
        printk("USB host initialization failed: %d\n", ret);
        return;
    }
    for (;;) {
        midi_host_poll();
        k_msleep(1);
    }
}

int play_status_note(DualSynth &synth, int note)
{
    // Render a brief note before accepting live keys. Use the same audio path
    // and gain as normal playback so the two audible checkpoints are meaningful.
    synth.reset();
    synth.noteOn(note, 80);
    for (unsigned i = 0; i < 34; ++i) {
        if (i == 19) synth.noteOn(note, 0);
        const int ret = queue_audio(&synth);
        if (ret) return ret;
    }
    synth.reset();
    return 0;
}

}

extern "C" void midi_event_push(const struct midi_event *event)
{
    if (k_msgq_put(&midi_events, event, K_NO_WAIT) != 0) {
        k_msgq_purge(&midi_events);
        midi_event reset{};
        reset.type = MIDI_RESET;
        k_msgq_put(&midi_events, &reset, K_NO_WAIT);
        k_msgq_put(&midi_events, event, K_NO_WAIT);
    }
}

int main()
{
    static DualSynth synth;
    static MidiInput midi(synth);
    int ret = audio_start();
    if (ret) {
        printk("Headphone output initialization failed: %d\n", ret);
        return ret;
    }
    ret = play_status_note(synth, 60);
    if (ret) {
        codec_write(dac_control3, dac_mute);
        i2s_trigger(i2s, I2S_DIR_TX, I2S_TRIGGER_DROP);
        printk("Startup tone failed: %d\n", ret);
        return ret;
    }
    k_thread_create(&midi_thread, midi_stack, K_THREAD_STACK_SIZEOF(midi_stack),
                    midi_receiver, nullptr, nullptr, nullptr, 5, 0, K_NO_WAIT);
    printk("PISCIS ready: all MIDI channels, ES8388 44100 Hz\n");
    for (;;) {
        midi_event event;
        for (unsigned i = 0; i < 32 && k_msgq_get(&midi_events, &event, K_NO_WAIT) == 0; ++i) {
            if (event.type == MIDI_CONNECTED) {
                midi.reset();
                printk("MIDI keyboard ready, IN=0x%02x\n", event.key);
                ret = play_status_note(synth, 72);
                if (ret) break;
            } else if (event.type == MIDI_DISCONNECTED || event.type == MIDI_RESET) {
                midi.reset();
            } else if (event.type == MIDI_ERROR) {
                printk("MIDI input error: %d\n", event.detail);
            } else if (event.type == MIDI_VOLUME) {
                playback_volume.set(event.value);
            } else {
                midi.apply(event);
            }
        }
        if (!ret) ret = queue_audio(&synth);
        if (ret) {
            codec_write(dac_control3, dac_mute);
            i2s_trigger(i2s, I2S_DIR_TX, I2S_TRIGGER_DROP);
            printk("Audio output stopped: %d\n", ret);
            return ret;
        }
    }
}
