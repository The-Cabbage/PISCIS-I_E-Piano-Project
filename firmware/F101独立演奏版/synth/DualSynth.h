#pragma once
#include "mdaEPiano.h"
#include "Organ.h"

// Shared by the F101 firmware and the optional PC preview. No allocations while rendering.
class DualSynth {
    mdaEPiano piano;
    hammond::Organ organ;
    int mode_=0,preset_=0,pianoGain_=100,organGain_=100;
public:
    DualSynth(){
        piano.setLoudness(1.25f);piano.setDecay(.4f);piano.setRelease(.3f);
        piano.setHardness(.4f);piano.setTreble(.20f);piano.setPanTremolo(.5f);
        piano.setDetune(0.f);piano.setMaxPolyphony(32);
    }
    void reset(){piano.processMidiController(64,0);piano.reset_voices();organ.reset();}
    // Returns true when a timbre change cleared the previous notes.
    bool configure(int mode,int preset,int pianoGain,int organGain){
        mode=hammond::clamp(mode,0,2);preset=hammond::clamp(preset,0,3);
        bool changed=mode!=mode_||preset!=preset_;
        if(changed){reset();mode_=mode;preset_=preset;organ.preset(preset);
            piano.setMaxPolyphony(mode==2?16:32);organ.setMaxVoices(mode==2?16:32);}
        pianoGain_=hammond::clamp(pianoGain,0,100);organGain_=hammond::clamp(organGain,0,100);
        return changed;
    }
    void noteOn(int n,int velocity){
        if(n<0||n>127)return;
        velocity=hammond::clamp(velocity,0,127);
        if(mode_!=1)piano.noteOn(n,velocity);
        if(mode_!=0)organ.noteOn(n,velocity);
    }
    void sustain(bool on){if(mode_!=1)piano.processMidiController(64,on?127:0);if(mode_!=0)organ.sustain(on);}
    int activeVoices()const{return (mode_!=1?piano.getActiveVoices():0)+(mode_!=0?organ.activeVoices():0);}
    void render(float *left, float *right, unsigned frames)
    {
        const float piano_gain = pianoGain_ * 0.01f;
        const float organ_gain = organGain_ * 0.01f;
        if (mode_ != 1) piano.processFloat(right, left, frames);
        if (mode_ == 0) {
            for (unsigned i = 0; i < frames; ++i) {
                left[i] *= piano_gain;
                right[i] *= piano_gain;
            }
            return;
        }
        for (unsigned i = 0; i < frames; ++i) {
            float organ_left, organ_right;
            organ.sample(organ_left, organ_right);
            if (mode_ == 1) {
                left[i] = organ_left * organ_gain;
                right[i] = organ_right * organ_gain;
            } else {
                left[i] = 0.5f * (left[i] * piano_gain + organ_left * organ_gain);
                right[i] = 0.5f * (right[i] * piano_gain + organ_right * organ_gain);
            }
        }
    }
};
