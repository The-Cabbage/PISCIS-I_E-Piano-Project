// MdaEPianoEngine.cpp
#include "MdaEPianoEngine.h"

MdaEPianoEngine::MdaEPianoEngine() {
    // 初始化 MDA 引擎参数（使用公有方法）
    engine.setLoudness(0.8f);   // 音量
    engine.setDecay(0.4f);
    engine.setRelease(0.3f);
    engine.setHardness(0.6f);
    engine.setTreble(0.5f);

    // ---- 混响参数 ----
    reverb.setRoomsize(0.8f);
    reverb.setDamping(0.5f);
    reverb.setWetLevel(0.3f);

    // ---- 合唱参数（温暖合唱） ----
    chorus.setDelayMs(25.0f);
    chorus.setModulationRate(1.2f);
    chorus.setModulationDepth(0.55f);
    chorus.setFeedback(0.25f);
    chorus.setMix(0.4f);
}

void MdaEPianoEngine::noteOn(int32_t note, int32_t velocity) {
    engine.noteOn(note, velocity);
}

void MdaEPianoEngine::noteOff(int32_t note) {
    engine.noteOn(note, 0);
}

void MdaEPianoEngine::allNotesOff() {
    engine.stop_voices();
}

void MdaEPianoEngine::sustainPedal(bool pressed) {
    // 通过 MIDI CC 实现延音
    engine.processMidiController(0x40, pressed ? 127 : 0);
}

void MdaEPianoEngine::setVolume(float volume) {
    // 音量范围 0.0 ~ 1.0
    engine.setLoudness(volume * 0.8f + 0.2f);
}

void MdaEPianoEngine::setParameter(int index, float value) {
    // 直接转发给引擎的公有方法（如果有）
    // 目前引擎没有公开 setParameter，所以留空
    // 可扩展为通过 switch 调用具体的 set 方法
}

void MdaEPianoEngine::render(int16_t* outputL, int16_t* outputR, uint32_t numSamples) {
    // 注意：mdaEPiano::process 的参数顺序是 (右, 左)
    // 并且它内部使用 AUDIO_BLOCK_SAMPLES 作为块大小
    // 如果 numSamples 与 AUDIO_BLOCK_SAMPLES 不一致，需要分块处理
    // 这里假设 numSamples == AUDIO_BLOCK_SAMPLES（由 config_hw.h 定义）

    // 直接渲染（不开启效果器，降低 CPU 负载）
    engine.process(outputR, outputL);

    // 如果需要效果器，可在此处添加：
    // int16_t wetL[AUDIO_BLOCK_SAMPLES], wetR[AUDIO_BLOCK_SAMPLES];
    // chorus.process(outputL, outputR, wetL, wetR, numSamples);
    // reverb.process(wetL, wetR, outputL, outputR, numSamples);
}

int32_t MdaEPianoEngine::getActiveVoices() const {
    return engine.getActiveVoices();
}

void MdaEPianoEngine::reset() {
    engine.reset_voices();
}