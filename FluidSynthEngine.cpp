// FluidSynthEngine.cpp - 修复变量名与 .h 一致
#include "FluidSynthEngine.h"
#include <iostream>
#include <cmath>
#include <cstring>

FluidSynthEngine::FluidSynthEngine(const std::string& soundfontPath,
                                   const std::string& midiPath,
                                   int sampleRate,
                                   int blockSize,
                                   float gain)
    : m_sampleRate(sampleRate),
      m_blockSize(blockSize),
      m_gain(gain),
      m_playing(false),
      m_floatBuffer(nullptr)
{
    m_settings = new_fluid_settings();
    if (!m_settings) {
        std::cerr << "❌ FluidSynthEngine: 创建设置失败" << std::endl;
        return;
    }

    fluid_settings_setnum(m_settings, "synth.sample-rate", sampleRate);
    fluid_settings_setnum(m_settings, "synth.gain", gain);
    fluid_settings_setint(m_settings, "synth.polyphony", 256);

    m_synth = new_fluid_synth(m_settings);
    if (!m_synth) {
        std::cerr << "❌ FluidSynthEngine: 创建合成器失败" << std::endl;
        return;
    }

    int sfid = fluid_synth_sfload(m_synth, soundfontPath.c_str(), 1);
    if (sfid == -1) {
        std::cerr << "❌ FluidSynthEngine: 加载 SoundFont 失败: " << soundfontPath << std::endl;
        return;
    }
    std::cout << "✅ FluidSynthEngine: SoundFont 加载成功" << std::endl;

    m_player = new_fluid_player(m_synth);
    if (!m_player) {
        std::cerr << "❌ FluidSynthEngine: 创建播放器失败" << std::endl;
        return;
    }

    int addResult = fluid_player_add(m_player, midiPath.c_str());
    if (addResult != FLUID_OK) {
        std::cerr << "❌ FluidSynthEngine: 添加 MIDI 失败: " << midiPath << std::endl;
        return;
    }
    std::cout << "✅ FluidSynthEngine: MIDI 加载成功" << std::endl;

    m_floatBuffer = new float[m_blockSize * 2];
    if (!m_floatBuffer) {
        std::cerr << "❌ FluidSynthEngine: 分配缓冲区失败" << std::endl;
        return;
    }

    fluid_player_play(m_player);
    m_playing = true;
    std::cout << "▶️ FluidSynthEngine: 开始播放" << std::endl;
}

FluidSynthEngine::~FluidSynthEngine() {
    if (m_player) {
        fluid_player_stop(m_player);
        delete_fluid_player(m_player);
    }
    if (m_synth) delete_fluid_synth(m_synth);
    if (m_settings) delete_fluid_settings(m_settings);
    if (m_floatBuffer) delete[] m_floatBuffer;
}

void FluidSynthEngine::noteOn(int32_t note, int32_t velocity) {
    // 由 MIDI 文件驱动，留空
}

void FluidSynthEngine::noteOff(int32_t note) {
    // 由 MIDI 文件驱动，留空
}

void FluidSynthEngine::allNotesOff() {
    fluid_synth_all_notes_off(m_synth, -1);
}

void FluidSynthEngine::sustainPedal(bool pressed) {
    // 暂不实现
}

void FluidSynthEngine::setVolume(float volume) {
    m_gain = volume;
    fluid_synth_set_gain(m_synth, m_gain);
}

void FluidSynthEngine::setParameter(int index, float value) {
    // 可扩展
}

void FluidSynthEngine::render(int16_t* outputL, int16_t* outputR, uint32_t numSamples) {
    if (!m_playing || !m_synth || !m_floatBuffer) {
        memset(outputL, 0, numSamples * sizeof(int16_t));
        memset(outputR, 0, numSamples * sizeof(int16_t));
        return;
    }

    // 检查播放器是否已完成
    if (m_player && fluid_player_get_status(m_player) == FLUID_PLAYER_DONE) {
        m_playing = false;
        memset(outputL, 0, numSamples * sizeof(int16_t));
        memset(outputR, 0, numSamples * sizeof(int16_t));
        return;
    }

    uint32_t processed = 0;
    while (processed < numSamples) {
        uint32_t remaining = numSamples - processed;
        uint32_t samplesThisBlock = (remaining < static_cast<uint32_t>(m_blockSize))
                                    ? remaining : m_blockSize;

        int result = fluid_synth_write_float(m_synth, samplesThisBlock,
                                             m_floatBuffer, 0, 2,
                                             m_floatBuffer, 1, 2);
        if (result != FLUID_OK) {
            std::cerr << "⚠️ FluidSynthEngine: 渲染失败，停止播放" << std::endl;
            m_playing = false;
            for (uint32_t i = processed; i < numSamples; ++i) {
                outputL[i] = 0;
                outputR[i] = 0;
            }
            return;
        }

        for (uint32_t i = 0; i < samplesThisBlock; ++i) {
            float valL = m_floatBuffer[i * 2];
            float valR = m_floatBuffer[i * 2 + 1];
            if (valL > 1.0f) valL = 1.0f;
            if (valL < -1.0f) valL = -1.0f;
            if (valR > 1.0f) valR = 1.0f;
            if (valR < -1.0f) valR = -1.0f;
            outputL[processed + i] = static_cast<int16_t>(valL * 32767.0f);
            outputR[processed + i] = static_cast<int16_t>(valR * 32767.0f);
        }

        processed += samplesThisBlock;
    }
}

int32_t FluidSynthEngine::getActiveVoices() const {
    if (m_synth) {
        return fluid_synth_get_active_voice_count(m_synth);
    }
    return 0;
}

void FluidSynthEngine::reset() {
    if (m_player) {
        fluid_player_stop(m_player);
        fluid_player_play(m_player);
        m_playing = true;
    }
}
