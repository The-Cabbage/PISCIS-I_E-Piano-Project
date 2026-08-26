// MdaEPianoEngine.cpp
#include "MdaEPianoEngine.h"
#include <cmath>

MdaEPianoEngine::MdaEPianoEngine() {
    engine.setLoudness(0.8f);
    engine.setDecay(0.4f);
    engine.setRelease(0.3f);
    engine.setHardness(0.6f);
    engine.setTreble(0.5f);

    reverb.setRoomsize(0.8f);
    reverb.setDamping(0.5f);
    reverb.setWetLevel(0.3f);

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
    engine.processMidiController(0x40, pressed ? 127 : 0);
}

void MdaEPianoEngine::setVolume(float volume) {
    engine.setLoudness(volume * 0.8f + 0.2f);
}

void MdaEPianoEngine::setParameter(int index, float value) {
    switch (index) {
        case MDA_EP_DECAY:
            engine.setDecay(value);
            break;
        case MDA_EP_RELEASE:
            engine.setRelease(value);
            break;
        case MDA_EP_HARDNESS:
            engine.setHardness(value);
            break;
        case MDA_EP_TREBLE:
            engine.setTreble(value);
            break;
        case MDA_EP_PAN_TREM:
            engine.setPanTremolo(value);
            break;
        case MDA_EP_LFO_RATE:
            engine.setPanLFO(value);
            break;
        case MDA_EP_VELOCITY_SENSE:
            engine.setVelocitySense(value);
            break;
        case MDA_EP_STEREO:
            engine.setStereo(value);
            break;
        case MDA_EP_MAX_POLY:
            // value 范围 0~1，映射到 1~NVOICES
            {
                uint8_t poly = static_cast<uint8_t>(value * (NVOICES - 1)) + 1;
                engine.setMaxPolyphony(poly);
            }
            break;
        case MDA_EP_TUNE:
            engine.setTune(value);
            break;
        case MDA_EP_DETUNE:
            engine.setDetune(value);
            break;
        case MDA_EP_OVERDRIVE:
            engine.setOverdrive(value);
            break;
        default:
            // 未知参数，忽略
            break;
    }
}

// ============================================================
// render：调用引擎处理，注意参数顺序
// ============================================================
void MdaEPianoEngine::render(int16_t* outputL, int16_t* outputR, uint32_t numSamples) {
    // mdaEPiano::process 参数顺序是 (右, 左)
    engine.process(outputR, outputL);
}

int32_t MdaEPianoEngine::getActiveVoices() const {
    return engine.getActiveVoices();
}

void MdaEPianoEngine::reset() {
    engine.reset_voices();
}