// AudioEngineInterface.h
#ifndef AUDIO_ENGINE_INTERFACE_H
#define AUDIO_ENGINE_INTERFACE_H

#include <cstdint>

/**
 * @brief 音源引擎抽象接口
 * 
 * 所有音源引擎（MDA EPiano、FluidSynth、自研等）都必须实现此接口。
 * 上层代码（MIDI处理、音频输出）只依赖此接口，不依赖具体实现。
 */
class AudioEngineInterface {
public:
    virtual ~AudioEngineInterface() = default;

    // MIDI 事件
    virtual void noteOn(int32_t note, int32_t velocity) = 0;
    virtual void noteOff(int32_t note) = 0;
    virtual void allNotesOff() = 0;
    virtual void sustainPedal(bool pressed) = 0;

    // 参数控制
    virtual void setVolume(float volume) = 0;
    virtual void setParameter(int index, float value) = 0;

    // 音频渲染
    virtual void render(int16_t* outputL, int16_t* outputR, uint32_t numSamples) = 0;

    // 状态查询
    virtual int32_t getActiveVoices() const = 0;
    virtual void reset() = 0;

    // ✅ 新增：检查是否正在播放（用于 FluidSynth）
    virtual bool isPlaying() const = 0;
};

#endif