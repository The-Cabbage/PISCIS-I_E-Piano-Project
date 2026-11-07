/*
   MicroMDAEPiano

   MicroMDAEPiano is a port of the MDA-EPiano sound engine
   (https://sourceforge.net/projects/mda-vst/) for the Teensy-3.5/3.6 with audio shield.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "config.h"
#ifdef USE_XFADE_DATA
#include "mdaEPianoDataXfade.h"
#else
#include "mdaEPianoData.h"
#endif
#include "mdaEPiano.h"
#include <stdio.h>
#include <math.h>

// ========== 为 PC 测试环境添加 ==========
static int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void set_master_volume(uint8_t value) { }
config_t configuration = {0};
uint8_t master_volume = 80;
// ========================================

extern void set_master_volume(uint8_t value);
extern uint8_t master_volume;
extern config_t configuration;

// ==================== 构造函数 ====================
mdaEPiano::mdaEPiano()
{
    programs = new mdaEPianoProgram[NPROGS]{};
    Fs = SAMPLE_RATE;  iFs = 1.0f / Fs;

    setDecay(0.500f);
    setRelease(0.500f);
    setHardness(0.500f);
    setTreble(0.500f);
    setPanTremolo(0.500f);
    setPanLFO(0.650f);
    setVelocitySense(0.250f);
    setStereo(0.500f);
    setMaxPolyphony(NVOICES);
    setTune(0.500f);
    setDetune(0.146f);
    setOverdrive(0.000f);
    setLoudness(0.64616f);

    waves = (short*)epianoData;
    printf("✅ waves 地址: %p\n", waves);
    printf("✅ epianoData[0] = %d\n", epianoData[0]);
    printf("✅ epianoData[1] = %d\n", epianoData[1]);

    // ---- keygroup 映射表 ----
    kgrp[ 0].root = 36;  kgrp[ 0].high = 39;
    kgrp[ 3].root = 43;  kgrp[ 3].high = 45;
    kgrp[ 6].root = 48;  kgrp[ 6].high = 51;
    kgrp[ 9].root = 55;  kgrp[ 9].high = 57;
    kgrp[12].root = 60;  kgrp[12].high = 63;
    kgrp[15].root = 67;  kgrp[15].high = 69;
    kgrp[18].root = 72;  kgrp[18].high = 75;
    kgrp[21].root = 79;  kgrp[21].high = 81;
    kgrp[24].root = 84;  kgrp[24].high = 87;
    kgrp[27].root = 91;  kgrp[27].high = 93;
    kgrp[30].root = 96;  kgrp[30].high = 999;

    kgrp[0].pos = 0;        kgrp[0].end = 8476;     kgrp[0].loop = 4400;
    kgrp[1].pos = 8477;     kgrp[1].end = 16248;    kgrp[1].loop = 4903;
    kgrp[2].pos = 16249;    kgrp[2].end = 34565;    kgrp[2].loop = 6398;
    kgrp[3].pos = 34566;    kgrp[3].end = 41384;    kgrp[3].loop = 3938;
    kgrp[4].pos = 41385;    kgrp[4].end = 45760;    kgrp[4].loop = 1633;
    kgrp[5].pos = 45761;    kgrp[5].end = 65211;    kgrp[5].loop = 5245;
    kgrp[6].pos = 65212;    kgrp[6].end = 72897;    kgrp[6].loop = 2937;
    kgrp[7].pos = 72898;    kgrp[7].end = 78626;    kgrp[7].loop = 2203;
    kgrp[8].pos = 78627;    kgrp[8].end = 100387;   kgrp[8].loop = 6368;
    kgrp[9].pos = 100388;   kgrp[9].end = 116297;   kgrp[9].loop = 10452;
    kgrp[10].pos = 116298;  kgrp[10].end = 127661;  kgrp[10].loop = 5217;
    kgrp[11].pos = 127662;  kgrp[11].end = 144113;  kgrp[11].loop = 3099;
    kgrp[12].pos = 144114;  kgrp[12].end = 152863;  kgrp[12].loop = 4284;
    kgrp[13].pos = 152864;  kgrp[13].end = 173107;  kgrp[13].loop = 3916;
    kgrp[14].pos = 173108;  kgrp[14].end = 192734;  kgrp[14].loop = 2937;
    kgrp[15].pos = 192735;  kgrp[15].end = 204598;  kgrp[15].loop = 4732;
    kgrp[16].pos = 204599;  kgrp[16].end = 218995;  kgrp[16].loop = 4733;
    kgrp[17].pos = 218996;  kgrp[17].end = 233801;  kgrp[17].loop = 2285;
    kgrp[18].pos = 233802;  kgrp[18].end = 248011;  kgrp[18].loop = 4098;
    kgrp[19].pos = 248012;  kgrp[19].end = 265287;  kgrp[19].loop = 4099;
    kgrp[20].pos = 265288;  kgrp[20].end = 282255;  kgrp[20].loop = 3609;
    kgrp[21].pos = 282256;  kgrp[21].end = 293776;  kgrp[21].loop = 2446;
    kgrp[22].pos = 293777;  kgrp[22].end = 312566;  kgrp[22].loop = 6278;
    kgrp[23].pos = 312567;  kgrp[23].end = 330200;  kgrp[23].loop = 2283;
    kgrp[24].pos = 330201;  kgrp[24].end = 348889;  kgrp[24].loop = 2689;
    kgrp[25].pos = 348890;  kgrp[25].end = 365675;  kgrp[25].loop = 4370;
    kgrp[26].pos = 365676;  kgrp[26].end = 383661;  kgrp[26].loop = 5225;
    kgrp[27].pos = 383662;  kgrp[27].end = 393372;  kgrp[27].loop = 2811;
    kgrp[28].pos = 383662;  kgrp[28].end = 393372;  kgrp[28].loop = 2811;
    kgrp[29].pos = 393373;  kgrp[29].end = 406045;  kgrp[29].loop = 4522;
    kgrp[30].pos = 406046;  kgrp[30].end = 414486;  kgrp[30].loop = 2306;
    kgrp[31].pos = 406046;  kgrp[31].end = 414486;  kgrp[31].loop = 2306;
    kgrp[32].pos = 414487;  kgrp[32].end = 422408;  kgrp[32].loop = 2169;

    reset_voices();
}

// ==================== 析构函数 ====================
mdaEPiano::~mdaEPiano()
{
    if (programs) delete [] programs;
}

// ==================== 重置声部 ====================
void mdaEPiano::reset_voices()
{
    for (int32_t v = 0; v < NVOICES; v++) {
        voice[v].env = 0.0f;
        voice[v].dec = 0.99f;
    }
    muff = 160.0f;
    sustain = activevoices = 0;
    tl = tr = lfo0 = dlfo = 0.0f;
    lfo1 = 1.0f;
    update();
}

void mdaEPiano::reset_controllers()
{
    tl = tr = lfo0 = dlfo = 0.0f;
    lfo1 = 1.0f;
    update();
}

void mdaEPiano::stop_voices()
{
    for (int32_t v = 0; v < NVOICES; v++)
        voice[v].env = 0.0f;
    muff = 160.0f;
    activevoices = 0;
    update();
}

// ==================== 参数更新（当前全部禁用） ====================
void mdaEPiano::update()
{
    float* param = programs[0].param;

    // 1. 高频滤波
    size = (int32_t)(12.0f * param[2] - 6.0f);
    treb = 4.0f * param[3] * param[3] - 1.0f;
    if (param[3] > 0.5f) tfrq = 14000.0f; else tfrq = 5000.0f;
    tfrq = 1.0f - (float)exp(-iFs * tfrq);

    // 2. LFO
    rmod = lmod = param[4] + param[4] - 1.0f;
    if (param[4] < 0.5f) rmod = -rmod;
    dlfo = 6.283f * iFs * (float)exp(6.22f * param[5] - 2.61f);

    // 3. 力度响应
    velsens = 1.0f + param[6] + param[6];
    if (param[6] < 0.25f) velsens -= 0.75f - 3.0f * param[6];

    // 4. 立体声/微调/过载
    width = 0.03f * param[7];
    fine = param[9] - 0.5f;
    random = 0.077f * param[10] * param[10];
    stretch = 0.0f;
    overdrive = 1.8f * param[11];
}
// ==================== resume ====================
void mdaEPiano::resume()
{
    Fs = SAMPLE_RATE;
    iFs = 1.0f / Fs;
    dlfo = 6.283f * iFs * (float)exp(6.22f * programs[0].param[5] - 2.61f);
}

// ==================== 参数设置函数 ====================
void mdaEPiano::setDecay(float value) { setParameter(MDA_EP_DECAY, value); }
void mdaEPiano::setRelease(float value) { setParameter(MDA_EP_RELEASE, value); }
void mdaEPiano::setHardness(float value) { setParameter(MDA_EP_HARDNESS, value); }
void mdaEPiano::setTreble(float value) { setParameter(MDA_EP_TREBLE, value); }
void mdaEPiano::setPanTremolo(float value) { setParameter(MDA_EP_PAN_TREM, value); }
void mdaEPiano::setPanLFO(float value) { setParameter(MDA_EP_LFO_RATE, value); }
void mdaEPiano::setVelocitySense(float value) { setParameter(MDA_EP_VELOCITY_SENSE, value); }
void mdaEPiano::setStereo(float value) { setParameter(MDA_EP_STEREO, value); }
void mdaEPiano::setMaxPolyphony(uint8_t value) {
    if (value > NVOICES) value = NVOICES;
    if (value <= 0) value = 1;
    max_polyphony = value;
}
void mdaEPiano::setTune(float value) { setParameter(MDA_EP_TUNE, value); }
void mdaEPiano::setDetune(float value) { setParameter(MDA_EP_DETUNE, value); }
void mdaEPiano::setOverdrive(float value) { setParameter(MDA_EP_OVERDRIVE, value); }
void mdaEPiano::setLoudness(float value) { volume = value * 0.16f; }

// ==================== 通用参数设置 ====================
void mdaEPiano::setParameter(int32_t index, float value)
{
    programs[0].param[index] = value;
    update();
}

// ==================== 获取活动声部数 ====================
int32_t mdaEPiano::getActiveVoices() const
{
    return activevoices;
}

// ==================== 音频渲染 ====================
static inline void write_output(int16_t &out, float sample)
{
    if (sample > 1.0f) sample = 1.0f;
    else if (sample < -1.0f) sample = -1.0f;
    out = static_cast<int16_t>(sample * 0x7fff) >> REDUCE_LOUDNESS;
}

static inline void write_output(float &out, float sample)
{
    out = sample / static_cast<float>(1 << REDUCE_LOUDNESS);
}

template <typename Sample>
void mdaEPiano::processBlock(Sample* outputs_r, Sample* outputs_l, uint32_t numSamples)
{
    if (numSamples == 0) return;
    int16_t v;
    float x, l, r, od = overdrive;
    int32_t i;
    uint32_t frame;

    for (frame = 0; frame < numSamples; frame++)
    {
        VOICE *V = voice;
        l = r = 0.0f;
        for (v = 0; v < activevoices; v++)
        {
            V->frac += V->delta;
            V->pos += V->frac >> 16;
            V->frac &= 0xFFFF;
            while (V->pos > V->end)
                V->pos -= V->loop;
            // 边界保护
            if (V->pos < 0) V->pos = 0;
            if (V->pos >= WAVELEN - 1) V->pos = WAVELEN - 2;

            // 线性插值
            // Blend the last 50 samples with the matching loop-start segment.
            // Keep the source sample table read-only; no extra sample copy is needed.
            auto loopSample = [&](int32_t pos) -> int32_t {
                const int32_t distance = (V->end + 1) - pos;
                if (distance >= 0 && distance < 50) {
                    const float blend = 1.0f - 0.02f * distance;
                    return static_cast<int32_t>((1.0f - blend) * waves[pos]
                                               + blend * waves[pos - V->loop]);
                }
                return waves[pos];
            };
            const int32_t sample0 = loopSample(V->pos);
            const int32_t sample1 = loopSample(V->pos + 1);
            i = sample0 + ((static_cast<int64_t>(V->frac) * (sample1 - sample0)) >> 16);
            x = V->env * static_cast<float>(i) / 32768.0f;

            V->env = V->env * V->dec;
            if (x > 0.0f)
            {
                x -= od * x * x;
                if (x < -V->env) x = -V->env;
            }
            l += V->outl * x;
            r += V->outr * x;

            V++;
        }

        // 高频滤波
        tl += tfrq * (l - tl);
        tr += tfrq * (r - tr);
        r  += treb * (r - tr);
        l  += treb * (l - tl);

        // LFO
        lfo0 += dlfo * lfo1;
        lfo1 -= dlfo * lfo0;
        l += l * lmod * lfo1;
        r += r * rmod * lfo1;

        r *= 0.5;
        l *= 0.5;

        write_output(outputs_l[frame], l);
        write_output(outputs_r[frame], r);
    }

    if (fabs(tl) < 1.0e-10) tl = 0.0f;
    if (fabs(tr) < 1.0e-10) tr = 0.0f;

    for (v = 0; v < activevoices; v++)
        if (voice[v].env < SILENCE)
            voice[v] = voice[--activevoices];
}

void mdaEPiano::process(int16_t* outputs_r, int16_t* outputs_l, uint32_t numSamples)
{
    processBlock(outputs_r, outputs_l, numSamples);
}

void mdaEPiano::processFloat(float* outputs_r, float* outputs_l, uint32_t numSamples)
{
    processBlock(outputs_r, outputs_l, numSamples);
}

// ==================== 音符事件处理 ====================
void mdaEPiano::noteOn(int32_t note, int32_t velocity)
{
    float* param = programs[0].param;
    float l = 99.0f;
    int32_t  v, vl = 0, k, s;

    if (velocity > 0) {
        if (activevoices < max_polyphony) {
            vl = activevoices;
            activevoices++;
            voice[vl].f0 = voice[vl].f1 = 0.0f;
        } else {
            for (v = 0; v < max_polyphony; v++) {
                if (voice[v].env < l) {
                    l = voice[v].env;
                    vl = v;
                }
            }
        }

        k = (note - 60) * (note - 60);
        l = fine + random * ((float)(k % 13) - 6.5f);
        if (note > 60) l += stretch * (float)k;

        s = size;
        k = 0;
        while (note > (kgrp[k].high + s)) k += 3;
        l += (float)(note - kgrp[k].root);
        l = 32000.0f * iFs * (float)exp(0.05776226505 * l);
        voice[vl].delta = (int32_t)(65536.0f * l);
        voice[vl].frac = 0;

        if (velocity > 48) k++;
        if (velocity > 80) k++;
        voice[vl].pos = kgrp[k].pos;
        voice[vl].end = kgrp[k].end - 1;
        voice[vl].loop = kgrp[k].loop;

        voice[vl].env = (3.0f + 2.0f * velsens) * (float)pow(0.0078f * velocity, velsens);
        if (note > 60) voice[vl].env *= (float)exp(0.01f * (float)(60 - note));

        l = 50.0f + param[4] * param[4] * muff + muffvel * (float)(velocity - 64);
        if (l < (55.0f + 0.4f * (float)note)) l = 55.0f + 0.4f * (float)note;
        if (l > 210.0f) l = 210.0f;
        voice[vl].ff = l * l * iFs;

        voice[vl].note = note;
        if (note < 12) note = 12;
        if (note > 108) note = 108;
        l = volume;
        voice[vl].outr = l + l * width * (float)(note - 60);
        voice[vl].outl = l + l - voice[vl].outr;

        if (note < 44) note = 44;
        voice[vl].dec = (float)exp(-iFs * exp(-1.0 + 0.03 * (double)note - 2.0f * param[0]));
    } else {
        for (v = 0; v < max_polyphony; v++) {
            if (voice[v].note == note) {
                if (sustain == 0)
                    voice[v].dec = (float)exp(-iFs * exp(6.0 + 0.01 * (double)note - 5.0 * param[1]));
                else
                    voice[v].note = SUSTAIN;
            }
        }
    }
}

// ==================== MIDI 控制器处理 ====================
bool mdaEPiano::processMidiController(uint8_t data1, uint8_t data2)
{
    float* param = programs[0].param;
    switch (data1) {
        case 0x01:  // mod wheel
            modwhl = 0.0078f * (float)(data2);
            if (modwhl > 0.05f) {
                rmod = lmod = modwhl;
                if (param[4] < 0.5f) rmod = -rmod;
            }
            break;
        case 0x40:  // sustain pedal
        case 0x42:  // sustenuto pedal
            sustain = data2 & 0x40;
            if (sustain == 0) noteOn(SUSTAIN, 0);
            break;
        case 0x78: reset_voices(); break;
        case 0x79: reset_controllers(); break;
        case 0x7b: stop_voices(); break;
        case 0x7e: setMaxPolyphony(1); break;
        case 0x7f: setMaxPolyphony(configuration.max_poly); break;
        default: break;
    }
    return true;
}
