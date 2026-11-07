#include <cstdint>
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

#ifndef __mdaEPiano__
#define __mdaEPiano__

//#include <Audio.h>
//#include <Arduino.h>
#include <string.h>
#include "config.h"

#define NPARAMS 12       ///< 音源参数数量，参数索引见下方 MDA_EP_* 定义。
#define NPROGS   1       ///< 内部音色槽数量；当前实现只使用一个程序。
#define NOUTS    2       ///< 输出声道数量：左、右两个声道。
#define SUSTAIN 128      ///< 用作“该声部正由延音踏板保持”的内部音符标记。
#define SILENCE 0.0001f  ///< 包络低于此阈值时，声部被视为静音并回收。
#define WAVELEN 422414   ///< 内置 EPiano 波形数据包含的 int16_t 采样点数量。

// MDAEPiano parameter mapping
#define MDA_EP_DECAY 0
#define MDA_EP_RELEASE 1
#define MDA_EP_HARDNESS 2
#define MDA_EP_TREBLE 3
#define MDA_EP_PAN_TREM 4
#define MDA_EP_LFO_RATE 5
#define MDA_EP_VELOCITY_SENSE 6
#define MDA_EP_STEREO 7
#define MDA_EP_MAX_POLY 8
#define MDA_EP_TUNE 9
#define MDA_EP_DETUNE 10
#define MDA_EP_OVERDRIVE 11

/**
 * @brief 保存一套 MDA EPiano 音色参数。
 *
 * 参数统一使用 float 保存，通常取值为 0.0～1.0。mdaEPiano 是其友元，
 * 外部代码应通过 mdaEPiano 的 set*() 接口修改参数，而不是直接访问数组。
 */
class mdaEPianoProgram
{
    friend class mdaEPiano;
  private:
    float param[NPARAMS];
};

/**
 * @brief 单个复音声部的实时播放状态。
 *
 * 每次 Note On 会创建或复用一个 VOICE。声部以 16.16 定点相位读取一段
 * PCM 采样，应用指数衰减包络后累加到左右输出。该结构处于音频热路径中，
 * 移植时应避免在渲染过程中动态分配或移动它。
 */
struct VOICE
{
  int32_t  delta;  ///< 每个输出帧推进的 16.16 定点采样相位增量，决定音高。
  int32_t  frac;   ///< 当前采样位置的小数部分，用于相邻 PCM 点之间的线性插值。
  int32_t  pos;    ///< 当前波形数组中的整数采样位置。
  int32_t  end;    ///< 当前采样段的末尾位置；越过后进入循环回绕。
  int32_t  loop;   ///< 从 end 回退的采样点数，即循环长度。

  float env;  ///< 当前包络幅度，同时用于判断声部是否已经衰减至静音。
  float dec;  ///< 每个采样点乘到 env 上的指数衰减系数。

  float f0;   ///< 原始 MDA 引擎的一阶低通状态；当前渲染代码未使用。
  float f1;   ///< 原始 MDA 引擎的一阶低通状态；当前渲染代码未使用。
  float ff;   ///< 为当前音符计算的滤波系数；当前渲染代码未使用。

  float outl; ///< 此声部混入左声道的增益。
  float outr; ///< 此声部混入右声道的增益。
  int32_t note; ///< 触发该声部的 MIDI 音符号，或内部的 SUSTAIN 标记。
};

/**
 * @brief 一个键盘/力度采样分区（keygroup）的描述信息。
 *
 * 相邻三个 KGRP 分别对应同一键盘区域的弱、中、强三层力度采样。
 */
struct KGRP
{
  int32_t root; ///< 该采样的原始 MIDI 根音；变调播放以它为基准。
  int32_t high; ///< 此键盘区域覆盖的最高 MIDI 音符。
  int32_t pos;  ///< 该采样在合并波形数组中的起始下标。
  int32_t end;  ///< 该采样在合并波形数组中的结束下标。
  int32_t loop; ///< 采样播放到末尾时向前回退的循环长度。
};

/**
 * @brief 基于多采样的 MDA 电钢琴音源核心。
 *
 * 音源根据音高选择键盘分区、根据力度选择三层采样之一，通过变速播放、
 * 指数包络、简单高频塑形、立体声展开、颤音/自动声像和过载生成双声道 PCM。
 * 类本身处理音符和音频，不负责 MIDI 端口、DAC/I2S、UI、混响或合唱效果。
 */
class mdaEPiano
{
  public:
    /** 初始化默认参数、采样映射、声部池和内部 DSP 状态。 */
    mdaEPiano();

    /** 释放构造函数创建的内部程序参数数组。 */
    ~mdaEPiano();

    /**
     * @brief 渲染指定帧数的立体声音频块，默认保持原有块大小。
     * @param outputs_r 右声道输出缓冲区，至少容纳 numSamples 个 int16_t。
     * @param outputs_l 左声道输出缓冲区，至少容纳 numSamples 个 int16_t。
     *
     * 函数会覆盖输出缓冲区。它遍历所有活动声部，进行采样插值、包络和
     * 音色处理，最后限幅并量化为 16-bit PCM。应从实时音频回调中调用。
     */
    virtual void process(int16_t *outputs_r, int16_t *outputs_l, uint32_t numSamples = AUDIO_BLOCK_SAMPLES);
    // Mix-bus output retains peaks above +/-1 for the final output limiter.
    void processFloat(float *outputs_r, float *outputs_l, uint32_t numSamples);

    /**
     * @brief 处理音符按下或释放。
     * @param note MIDI 音符号；正常范围为 0～127。
     * @param velocity MIDI 力度。大于 0 表示 Note On，0 表示 Note Off。
     *
     * Note On 时选择音高分区和力度层并分配声部；复音已满时窃取当前
     * 包络最小的声部。Note Off 时切换到释放衰减；踏板按下时延迟释放。
     */
    void noteOn(int32_t note, int32_t velocity);

    /**
     * @brief 处理音源原生支持的 MIDI Control Change 消息。
     * @param data1 MIDI CC 编号。
     * @param data2 MIDI CC 值，范围 0～127。
     * @return 当前实现始终返回 true。
     *
     * 支持调制轮、主音量、延音/延音保持、All Sound Off、Reset All
     * Controllers、All Notes Off，以及单音/复音模式切换。
     */
    virtual bool processMidiController(uint8_t data1, uint8_t data2);
    //virtual void setProgram(int32_t program);
    //virtual float getParameter(int32_t index);
    /**
     * @brief 在音频系统启动或采样率环境恢复后刷新采样率相关状态。
     *
     * 当前实现从 SAMPLE_RATE 重新设置 Fs/iFs，并重新计算 LFO 步进。
     */
    virtual void resume();

    /** 立即清空全部声部、延音状态和部分 DSP 状态，并重新应用参数。 */
    void reset_voices(void);

    /** 重置调制器等控制器状态，但保留当前正在播放的声部。 */
    void reset_controllers(void);

    /** 立即将所有声部标记为静音；不同于正常 Note Off，不保留释放尾音。 */
    void stop_voices(void);

    /** 设置按键后的自然衰减长度，value 通常为 0.0～1.0。 */
    void setDecay(float value);
    /** 设置 Note Off 后的释放时间，value 通常为 0.0～1.0。 */
    void setRelease(float value);
    /** 设置音色硬度；通过偏移采样键盘分区的选择边界改变音色。 */
    void setHardness(float value);
    /** 设置高频明暗，0.5 附近为中性，value 通常为 0.0～1.0。 */
    void setTreble(float value);
    /** 设置颤音/自动声像深度及模式，中心值 0.5 附近表示最小调制。 */
    void setPanTremolo(float value);
    /** 设置颤音/自动声像 LFO 频率，使用指数频率映射。 */
    void setPanLFO(float value);
    /** 设置 MIDI 力度到声部初始包络幅度的响应曲线。 */
    void setVelocitySense(float value);
    /** 设置按键位置在左右声道上的展开宽度。 */
    void setStereo(float value);
    /** 设置最大复音数；实现会把值限制到 1～NVOICES。 */
    void setMaxPolyphony(uint8_t value);
    /** 设置整体微调参数；0.5 为标称音高。 */
    void setTune(float value);
    /** 设置各音符的确定性轻微失谐量，用于增加自然感。 */
    void setDetune(float value);
    /** 设置二次非线性过载强度，value 通常为 0.0～1.0。 */
    void setOverdrive(float value);
    /** 设置音源内部输出增益；该值不等同于硬件 Codec 的主音量。 */
    void setLoudness(float value);
    /** @return 当前参与渲染的活动声部数量。 */
    int32_t getActiveVoices(void) const;

  private:
    template <typename Sample>
    void processBlock(Sample *outputs_r, Sample *outputs_l, uint32_t numSamples);
    /** 将归一化参数转换为渲染所需的滤波、LFO、声像和失真系数。 */
    void update();

    /**
     * 填充一个程序的 12 个参数。当前版本仅保留声明，内置预置代码已停用。
     */
    void fillpatch(int32_t p, char *name, float p0, float p1, float p2, float p3, float p4,
                   float p5, float p6, float p7, float p8, float p9, float p10, float p11);

    /** 设置指定索引的归一化参数，并立即调用 update() 刷新 DSP 系数。 */
    void setParameter(int32_t index, float value);

    mdaEPianoProgram* programs; ///< 程序参数数组；当前只分配 NPROGS == 1 个。
    float Fs;                   ///< 当前输出采样率，单位 Hz。
    float iFs;                  ///< 采样率倒数，用于按采样点计算时间相关系数。

    uint8_t max_polyphony; ///< 用户设置的最大同时发声数，不超过 NVOICES。
    KGRP kgrp[34];         ///< 键盘分区及三层力度采样的映射表。
    VOICE voice[NVOICES];  ///< 固定容量的实时声部池。
    int32_t activevoices;  ///< voice[] 前部当前有效的声部数量。
    short *waves;          ///< 指向合并后的 16-bit 单声道 EPiano PCM 采样数据。
    float width;           ///< 根据 MIDI 音高计算左右声像的立体声展开系数。
    int32_t size;          ///< Hardness 转换出的 keygroup 选择边界偏移。
    int32_t sustain;       ///< 延音踏板状态；0 表示释放，非零表示按下。
    float lfo0, lfo1;      ///< 正交递推振荡器状态，用于 tremolo/autopan。
    float dlfo;            ///< LFO 每采样点的相位步进系数。
    float lmod, rmod;      ///< 左右声道 LFO 调制深度；符号决定 tremolo 或 autopan。
    float treb;            ///< 高频增强或衰减增益。
    float tfrq;            ///< 一阶高频塑形滤波器系数。
    float tl, tr;          ///< 左右声道高频塑形滤波器的历史状态。
    float tune;            ///< 原始引擎遗留的调音状态；当前渲染路径未直接使用。
    float fine;            ///< 由 Tune 参数得到的半音微调偏移。
    float random;          ///< 由 Detune 参数得到的逐音符确定性失谐强度。
    float stretch;         ///< 高音音律拉伸系数；当前实现固定为 0。
    float overdrive;       ///< 渲染时二次非线性失真的系数。
    float muff;            ///< 原始 MDA 的音色闷度状态，参与 Note On 时的系数计算。
    float muffvel;         ///< 原始 MDA 的力度相关闷度系数。
    float sizevel;         ///< 原始 MDA 的力度相关 hardness 系数；当前路径未使用。
    float velsens;         ///< MIDI 力度映射到初始包络的指数。
    float modwhl;          ///< 最近一次调制轮归一化值。
    float volume;          ///< 每个新声部使用的基础输出增益。
    float vol;             ///< 原始引擎遗留的音量状态；当前渲染路径未使用。
    //uint8_t curProgram;
};

#endif
