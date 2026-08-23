// config_hw.h - 专为硬件移植精简配置，不依赖 Arduino
#ifndef CONFIG_HW_H
#define CONFIG_HW_H

#include <cstdint>
#include <cstddef>

// ==================== 音频参数 ====================
#define AUDIO_SAMPLE_RATE       44100   // 采样率（Hz）
#define AUDIO_BLOCK_SAMPLES     256     // 每块采样数（ALSA period size）
#define NVOICES                 24      // 最大复音数（根据 CPU 调整）
#define REDUCE_LOUDNESS         0       // 不衰减

// ==================== MDA EPiano 参数索引 ====================
#define MDA_EP_DECAY            0
#define MDA_EP_RELEASE          1
#define MDA_EP_HARDNESS         2
#define MDA_EP_TREBLE           3
#define MDA_EP_PAN_TREM         4
#define MDA_EP_LFO_RATE         5
#define MDA_EP_VELOCITY_SENSE   6
#define MDA_EP_STEREO           7
#define MDA_EP_MAX_POLY         8
#define MDA_EP_TUNE             9
#define MDA_EP_DETUNE           10
#define MDA_EP_OVERDRIVE        11

// ==================== 消除 Arduino 依赖 ====================
#define PROGMEM
#define pgm_read_word(addr) (*(addr))

// ==================== 原 config.h 中的全局结构（移植所需） ====================
struct config_t {
    uint32_t checksum;
    uint8_t decay;
    uint8_t release;
    uint8_t hardness;
    uint8_t treble;
    int8_t stereo;
    int8_t transpose;
    int8_t tune;
    uint8_t detune;
    uint8_t velocity_sense;
    uint8_t pan_trem_frequency;
    uint8_t pan_trem_level;
    uint8_t overdrive;
    uint8_t comp_gain;
    uint8_t comp_response;
    bool comp_limit;
    uint8_t comp_threshold;
    uint8_t comp_attack;
    uint8_t comp_decay;
    uint8_t reverb_roomsize;
    uint8_t reverb_damping;
    uint8_t reverb_level;
    uint8_t chorus_frequency;
    uint8_t chorus_intensity;
    uint8_t chorus_waveform;
    uint8_t chorus_level;
    uint8_t bass_lr_level;
    uint8_t bass_mono_level;
    int8_t eq_bass;
    int8_t eq_treble;
    uint8_t loudness;
    uint8_t midi_channel;
    bool midi_soft_thru;
    uint8_t max_poly;
    uint8_t mono;
    int8_t pan;
};

// 外部声明（将在主程序中定义）
extern config_t configuration;
extern uint8_t master_volume;
extern void set_master_volume(uint8_t value);

// ==================== 辅助函数 ====================
static inline int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ==================== 兼容原 mdaEPiano.cpp 中的宏 ====================
#define SAMPLE_RATE AUDIO_SAMPLE_RATE

#endif // CONFIG_HW_H