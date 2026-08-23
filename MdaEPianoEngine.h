// MdaEPianoEngine.h
#ifndef MDA_EPIANO_ENGINE_H
#define MDA_EPIANO_ENGINE_H

#include "AudioEngineInterface.h"
#include "mdaEPiano.h"
#include "Freeverb.hpp"
#include "ChorusProcessor.hpp"

class MdaEPianoEngine : public AudioEngineInterface {
public:
    MdaEPianoEngine();
    ~MdaEPianoEngine() override = default;

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

    // ✅ 新增：检查是否正在播放（MDA 引擎始终返回 true）
    bool isPlaying() const override { return true; }

    // 混响控制
    void setReverbRoomsize(float value) { reverb.setRoomsize(value); }
    void setReverbDamping(float value)  { reverb.setDamping(value); }
    void setReverbWetLevel(float value) { reverb.setWetLevel(value); }

    // 合唱控制
    void setChorusDelay(float ms)       { chorus.setDelayMs(ms); }
    void setChorusRate(float rate)      { chorus.setModulationRate(rate); }
    void setChorusDepth(float depth)    { chorus.setModulationDepth(depth); }
    void setChorusFeedback(float fb)    { chorus.setFeedback(fb); }
    void setChorusMix(float mix)        { chorus.setMix(mix); }

private:
    mdaEPiano engine;
    FreeverbProcessor reverb;
    ChorusProcessor chorus;
};

#endif