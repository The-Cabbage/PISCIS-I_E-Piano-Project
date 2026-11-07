// C++ port of gpasquero/hammond-b3, MIT; see third_party/hammond-b3/LICENSE.
#pragma once
#include <cstdint>
#include <math.h>

#include <string.h>

namespace hammond {
template<class T> static T minimum(T a,T b){return a<b?a:b;}
template<class T> static T maximum(T a,T b){return a>b?a:b;}
template<class T> static T clamp(T x,T lo,T hi){return minimum(hi,maximum(lo,x));}
struct Patch {
    uint8_t bars[9]={8,8,8,0,0,0,0,0,0};
    bool percussion=true,soft=false,fast=true,third=true;
    int vibrato=0,leslie=1; // off,V1,V2,V3,C1,C2,C3; brake,slow,fast
    float drive=0.f,volume=.85f,toneHz=3900.f,tremoloDepth=.08f;
};
class Organ {
    static constexpr float rate=44100.f, tau=6.283185307179586f;
    static constexpr int tableSize=2048,delaySize=445,maxVoices=32;
    float sineTable[tableSize],shapeTable[tableSize+1];
    float phase[128]{},inc[128]{},wheel[128]{};
    struct Voice { int note=0; float env=0,velocity=0; bool gate=false; int reattackSamples=0; float accent=0,pedalRelease=0,tremDepth=0; uint32_t heldFrames=0; uint8_t wheelIds[9]{}; } voices[maxVoices];
    int activeBars=0;uint8_t barIndices[9]{};float barGains[9]{};
    bool keys[128]{}; int count=0,held=0,percNote=60,voiceLimit=maxVoices; bool pedal=false;
    float percEnv=0,atk,rel,accentDecay,percDecay,driveScale;
    // Bound the summed voice amplitude before waveshaping, especially for pure flute chords.
    static constexpr float outputGain=1.3462323f;
    float voiceGain=1.f,voiceGainTarget=1.f,headroomWeight=1.f;
    struct Delay {
        float data[delaySize]{}; int write=0;
        float process(float x,float delay){
            data[write]=x;float pos=float(write)-delay;if(pos<0)pos+=delaySize;
            int i=int(pos),j=i+1;if(j==delaySize)j=0;
            float out=data[i]+(data[j]-data[i])*(pos-i);
            if(++write==delaySize)write=0;
            return out;
        }
        void clear(){memset(data,0,sizeof(data));write=0;}
    } vibratoDelay,hornDelay,drumDelay;
    float tremPhase=0;
    float vibratoPhase=0,hornPhase=0,drumPhase=0,hornSpeed=.8f,drumSpeed=.7f;
    float hornAccel,drumAccel,lowpass=0,crossover;
    float tone1=0,tone2=0,toneCoeff=0,barScale=.1f,percAttack=0;
    Patch patch;
    float sine(float p)const{
#ifdef HAMMOND_REFERENCE_MATH
        return sinf(tau*p);
#else
        float x=p*tableSize;int i=int(x);float f=x-i;i&=tableSize-1;
        return sineTable[i]+(sineTable[(i+1)&(tableSize-1)]-sineTable[i])*f;
#endif
    }
    float shape(float x)const{
#ifdef HAMMOND_REFERENCE_MATH
        return tanhf(x);
#else
        float a=fabsf(x);if(a>=8.f)return x<0?-1.f:1.f;
        float p=a*(tableSize/8.f);int i=int(p);
        float v=shapeTable[i]+(shapeTable[i+1]-shapeTable[i])*(p-i);
        return x<0?-v:v;
#endif
    }
    float limit(float x)const{float a=fabsf(x);if(a<=.85f)return x;
        float v=.85f+.1f*shape((a-.85f)*10.f);return x<0?-v:v;}
    static void advance(float& p,float inc){p+=inc;if(p>=1.f)p-=1.f;}
public:
    static int foldback(int n){while(n<24)n+=12;while(n>114)n-=12;return n;}
    Organ(){
        for(int i=0;i<tableSize;++i)sineTable[i]=sinf(tau*i/tableSize);
        for(int i=0;i<=tableSize;++i)shapeTable[i]=tanhf(8.f*i/tableSize);
        for(int i=24;i<=114;++i)inc[i]=440.f*powf(2.f,(i-69)/12.f)/rate;
        atk=1.f-expf(-1.f/(.005f*rate));rel=1.f-expf(-1.f/(.035f*rate));
        accentDecay=expf(-1.f/(.06f*rate));
        hornAccel=1.f-expf(-1.f/(.8f*rate));drumAccel=1.f-expf(-1.f/(2.2f*rate));
        crossover=1.f-expf(-tau*800.f/rate);setPatch(Patch{});
    }
    void setPatch(Patch p){
        for(auto& b:p.bars)b=minimum<uint8_t>(8,b);
        p.vibrato=clamp(p.vibrato,0,6);p.leslie=clamp(p.leslie,0,2);
        p.drive=clamp(p.drive,0.f,1.f);p.volume=clamp(p.volume,0.f,1.f);
        p.toneHz=clamp(p.toneHz,1200.f,6000.f);p.tremoloDepth=clamp(p.tremoloDepth,0.f,.08f);
        toneCoeff=1.f-expf(-tau*p.toneHz/rate);
        patch=p;activeBars=0;float totalGain=0;
        for(int d=0;d<9;++d)if(p.bars[d]){barIndices[activeBars]=uint8_t(d);float gain=powf(2.f,(float(p.bars[d])-8.f)*.5f);barGains[activeBars++]=gain;totalGain+=gain;}
        // Normalize preset loudness without changing gain as notes enter or leave.
        barScale=.20f/maximum(1.f,totalGain);
        // Include each preset's output level in the budget. Leave room for percussion/effects.
        headroomWeight=.70f/maximum(.001f,barScale*totalGain*p.volume*outputGain);
        percDecay=expf(-1.f/((p.fast?.18f:.55f)*rate));
        driveScale=1.f/maximum(.05f,shape(1.f+p.drive*12.f));
    }
    void preset(int n){
        Patch p;
        // Jazz: hollow low drawbars and a clear short percussion transient.
        // Rock: strong octave harmonics, mild grit, centered without rotary motion.
        if(n==1){uint8_t b[9]={8,0,8,8,0,0,0,0,4};memcpy(p.bars,b,9);p.percussion=false;p.vibrato=0;p.leslie=0;p.drive=.12f;p.volume=.82f;p.toneHz=5000.f;}
        // Gospel: a fuller upper registration with gentle chorus and slow rotary.
        if(n==2){uint8_t b[9]={6,0,8,6,7,6,4,3,2};memcpy(p.bars,b,9);p.percussion=false;p.vibrato=4;p.leslie=1;p.drive=0;p.volume=.9f;p.toneHz=4500.f;}
        // Soft: pure 8-foot flute, no sub-octave, percussion or modulation.
        if(n==3){uint8_t b[9]={0,0,8,0,0,0,0,0,0};memcpy(p.bars,b,9);p.percussion=false;p.vibrato=0;p.leslie=0;p.drive=0;p.volume=.85f;p.toneHz=2200.f;p.tremoloDepth=0.f;}
        setPatch(p);
    }
    void sustain(bool on){pedal=on;}
    void setMaxVoices(int n){n=clamp(n,1,maxVoices);if(n!=voiceLimit){reset();voiceLimit=n;}}
    void noteOn(int n,int velocity){
        if(n<0||n>127)return;
        if(velocity<=0){if(keys[n]){keys[n]=false;--held;}for(int i=0;i<count;++i)if(voices[i].note==n)voices[i].gate=false;return;}
        bool fresh=!keys[n],silent=held==0;if(fresh){keys[n]=true;++held;}
        int at=0;while(at<count&&voices[at].note!=n)++at;
        const bool restrike=fresh&&at<count;
        if(at==count){if(count<voiceLimit)++count;else{at=0;for(int i=1;i<count;++i)if(voices[i].env<voices[at].env)at=i;}
            voices[at]=Voice{};voices[at].note=n;
            // Match the current MDA decay=0.4 envelope rate, with the same note clamp.
            // Applied after key release so physically held organ keys stay sustained.
            voices[at].pedalRelease=1.f-expf(-expf(-1.f+.03f*clamp(n,44,108)-.8f)/rate);
            static constexpr int offsets[9]={-12,7,0,12,19,24,28,31,36};
            for(int d=0;d<9;++d)voices[at].wheelIds[d]=uint8_t(foldback(n+offsets[d]));}
        // A pedal-held voice must articulate again on a fresh physical key press.
        // Fade its envelope down over 5 ms before attacking, avoiding a hard reset click.
        if(restrike)voices[at].reattackSamples=220;
        if(fresh){voices[at].accent=.35f;voices[at].heldFrames=0;}
        voices[at].gate=true;voices[at].velocity=maximum(.05f,minimum(127,velocity)/127.f);
        if(fresh&&(silent||restrike)){percEnv=1.f;percAttack=0;percNote=n;}
    }
    void reset(){
        count=held=0;pedal=false;memset(keys,0,sizeof(keys));percEnv=0;voiceGain=voiceGainTarget=1.f;tremPhase=0;
        vibratoDelay.clear();hornDelay.clear();drumDelay.clear();lowpass=tone1=tone2=percAttack=0;
    }
    int activeVoices()const{return count;}
    int heldKeys()const{return held;}
    void sample(float& l,float& r){
        for(int n=24;n<=114;++n){advance(phase[n],inc[n]);wheel[n]=sine(phase[n]);}
        // One shared 4 Hz oscillator keeps held chord notes moving together.
        advance(tremPhase,4.f/rate);const float tremWave=sine(tremPhase);
        float dry=0,voiceWeight=0;
        for(int i=0;i<count;){auto& v=voices[i];float target=v.gate?1.f:0.f;
            if(v.reattackSamples>0){v.env*=.975f;--v.reattackSamples;}
            else v.env+=(target-v.env)*(target>v.env?atk:(!v.gate&&pedal?v.pedalRelease:rel));
            v.accent*=accentDecay;
            // Physical key holds: wait 0.5 s, then fade to +/-8% over another 0.5 s.
            // A short note held only by the pedal does not start tremolo later.
            if(v.gate&&v.heldFrames<44100u)++v.heldFrames;
            const float tremTarget=v.gate&&v.heldFrames>22050u
                ?patch.tremoloDepth*float(v.heldFrames-22050u)/22050.f:0.f;
            v.tremDepth+=(tremTarget-v.tremDepth)*.002f;
            if(!v.gate&&v.env<1.e-4f){voices[i]=voices[--count];continue;}
            float sum=0;for(int d=0;d<activeBars;++d)sum+=wheel[v.wheelIds[barIndices[d]]]*barGains[d];
            const float amplitude=v.env*v.velocity*(1.f+v.accent);
            dry+=sum*amplitude*(1.f+v.tremDepth*tremWave);
            // Use maximum tremolo depth for the bound, not its oscillating waveform.
            voiceWeight+=amplitude*(1.f+v.tremDepth);++i;
        }
        // Continuous envelope-based headroom: no rounded voice counts and no audio-rate
        // waveform clipping to control chord volume. Attack envelopes make attenuation
        // continuous; slow recovery avoids pumping as notes are released.
        voiceGainTarget=minimum(1.f,headroomWeight/maximum(.0001f,voiceWeight));
        voiceGain=minimum(voiceGainTarget,voiceGain+(voiceGainTarget-voiceGain)*.00023f);
        dry*=barScale*voiceGain;
        // Soft percussion onset; no synthetic key-click noise or idle leakage.
        percAttack+=(1.f-percAttack)*atk;
        if(patch.percussion){dry+=wheel[foldback(percNote+(patch.third?19:12))]*percEnv*percAttack*(patch.soft?.045f:.08f);percEnv*=(!held&&!pedal)?(1.f-rel):percDecay;}
        advance(vibratoPhase,6.9f/rate);
        static constexpr float depths[7]={0,.04f,.07f,.1f,.04f,.07f,.1f};
        float wet=vibratoDelay.process(dry,rate*.0016f*(1.f+depths[patch.vibrato]*sine(vibratoPhase)));
        float mono=patch.vibrato==0?dry:(patch.vibrato>=4?.5f*(dry+wet):wet);
        if(patch.drive>0){float shaped=shape(mono*(1.f+patch.drive*12.f))*driveScale;mono=mono*(1.f-patch.drive)+shaped*patch.drive;}
        tone1+=toneCoeff*(mono-tone1);tone2+=toneCoeff*(tone1-tone2);mono=.8f*tone2+.2f*tone1;
        static constexpr float hornTargets[3]={0,.8f,6.8f},drumTargets[3]={0,.7f,6.f};
        hornSpeed+=(hornTargets[patch.leslie]-hornSpeed)*hornAccel;
        drumSpeed+=(drumTargets[patch.leslie]-drumSpeed)*drumAccel;
        advance(hornPhase,hornSpeed/rate);advance(drumPhase,drumSpeed/rate);
        float hs=sine(hornPhase),ds=sine(drumPhase);
        lowpass+=crossover*(mono-lowpass);
        float hd=hornDelay.process(mono-lowpass,rate*(.0015f+.00010f*(.5f+.5f*hs)));
        float dd=drumDelay.process(lowpass,rate*(.0015f+.00015f*(.5f+.5f*ds)));
        // Mostly centered direct sound, with a quiet narrow rotary image.
        // Brake bypasses modulation completely; stopped rotors must not wobble.
        hd*=.93f+.07f*hs;dd*=.96f+.04f*ds;
        float left=mono,right=mono;
        if(patch.leslie!=0){
            left=.8f*mono+.2f*(hd*(1.f-.12f*hs)+dd*(1.f-.08f*ds));
            right=.8f*mono+.2f*(hd*(1.f+.12f*hs)+dd*(1.f+.08f*ds));
        }
        // Reduce the previous 1.6 gain by 1.5 dB, before the existing soft limiter.
        l=limit(left*patch.volume*outputGain);r=limit(right*patch.volume*outputGain);
    }
    void render(int16_t* l,int16_t* r,unsigned frames){for(unsigned i=0;i<frames;++i){float a,b;sample(a,b);l[i]=int16_t(clamp(a,-1.f,1.f)*32767.f);r[i]=int16_t(clamp(b,-1.f,1.f)*32767.f);}}
};
}
