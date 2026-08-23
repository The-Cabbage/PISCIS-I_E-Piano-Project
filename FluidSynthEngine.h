// FluidSynthEngine.h
#ifndef FLUIDSYNTH_ENGINE_H
#define FLUIDSYNTH_ENGINE_H

#include "AudioEngineInterface.h"
#include <fluidsynth.h>
#include <string>
#include <atomic>

class FluidSynthEngine : public AudioEngineInterface {
public:
    FluidSynthEngine(const std::string& soundfontPath,
                     const std::string& midiPath,
                     int sampleRate = 44100,
                     int blockSize = 256,
                     float gain = 0.8f);
    ~FluidSynthEngine() override;

    // AudioEngineInterface 实现
    void noteOn(int32_t note, int32_t velocity) override;
    void noteOff(int32_t note) override;
    void allNotesOff() override;
    void sustainPedal(bool pressed) override;
    void setVolume(float volume) override;
    void setParameter(int index, float value) override;
    void render(int16_t* outputL, int16_t* outputR, uint32_t numSamples) override;
    int32_t getActiveVoices() const override;
    void reset() override;

    // ✅ 新增：检查是否正在播放（内联实现）
    bool isPlaying() const override { return m_playing; }

private:
    fluid_settings_t* m_settings;
    fluid_synth_t*    m_synth;
    fluid_player_t*   m_player;
    int               m_sampleRate;
    int               m_blockSize;
    float             m_gain;
    bool              m_playing;
    float*            m_floatBuffer;   // ✅ 添加这一行
};

#endif