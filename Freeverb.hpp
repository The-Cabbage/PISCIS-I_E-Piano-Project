// Freeverb.hpp - 纯C++混响处理器，无Teensy依赖
#ifndef FREEVERB_HPP
#define FREEVERB_HPP

#include <cstdint>
#include <cmath>
#include <cstring>

/**
 * @brief 16位整数饱和处理（原 sat16 函数，保持算法一致性）
 */
static inline int16_t sat16(int32_t n, int rshift) {
    if (n < 0) {
        n = n + (~(0xFFFFFFFFUL << rshift));
    }
    n = n >> rshift;
    if (n > 32767) return 32767;
    if (n < -32768) return -32768;
    return static_cast<int16_t>(n);
}

/**
 * @brief 纯C++ Freeverb混响处理器
 * 
 * 使用与 Teensy Audio Library 完全相同的算法和系数，
 * 但剥离了所有硬件依赖，可在任何平台编译运行。
 * 
 * 使用方式：
 *   FreeverbProcessor reverb(44100.0f);
 *   reverb.setRoomsize(0.8f);
 *   reverb.setDamping(0.5f);
 *   reverb.process(inL, inR, outL, outR, numSamples);
 */
class FreeverbProcessor {
public:
    /**
     * @param sampleRate 采样率（Hz），影响延迟线长度
     */
    FreeverbProcessor(float sampleRate = 44100.0f)
        : sampleRate(sampleRate)
    {
        // 根据采样率计算延迟线长度（保持与原始算法时间常数一致）
        // 原始Teensy代码使用固定长度（对应 44.1kHz 采样率）
        // 其他采样率需要按比例缩放
        float scale = sampleRate / 44100.0f;

        combLengths[0] = static_cast<int>(1116 * scale);
        combLengths[1] = static_cast<int>(1188 * scale);
        combLengths[2] = static_cast<int>(1277 * scale);
        combLengths[3] = static_cast<int>(1356 * scale);
        combLengths[4] = static_cast<int>(1422 * scale);
        combLengths[5] = static_cast<int>(1491 * scale);
        combLengths[6] = static_cast<int>(1557 * scale);
        combLengths[7] = static_cast<int>(1617 * scale);

        allpassLengths[0] = static_cast<int>(556 * scale);
        allpassLengths[1] = static_cast<int>(441 * scale);
        allpassLengths[2] = static_cast<int>(341 * scale);
        allpassLengths[3] = static_cast<int>(225 * scale);

        // 分配缓冲区并初始化
        for (int i = 0; i < 8; ++i) {
            combBufL[i] = new int16_t[combLengths[i]];
            combBufR[i] = new int16_t[combLengths[i]];
            std::memset(combBufL[i], 0, combLengths[i] * sizeof(int16_t));
            std::memset(combBufR[i], 0, combLengths[i] * sizeof(int16_t));
            combIndexL[i] = 0;
            combIndexR[i] = 0;
            combFilterL[i] = 0;
            combFilterR[i] = 0;
        }
        for (int i = 0; i < 4; ++i) {
            allpassBufL[i] = new int16_t[allpassLengths[i]];
            allpassBufR[i] = new int16_t[allpassLengths[i]];
            std::memset(allpassBufL[i], 0, allpassLengths[i] * sizeof(int16_t));
            std::memset(allpassBufR[i], 0, allpassLengths[i] * sizeof(int16_t));
            allpassIndexL[i] = 0;
            allpassIndexR[i] = 0;
        }

        // 默认参数（原始Freeverb标准值）
        setRoomsize(0.8f);
        setDamping(0.5f);
        setWetLevel(0.3f);
        setDryLevel(0.7f);
    }

    ~FreeverbProcessor() {
        for (int i = 0; i < 8; ++i) {
            delete[] combBufL[i];
            delete[] combBufR[i];
        }
        for (int i = 0; i < 4; ++i) {
            delete[] allpassBufL[i];
            delete[] allpassBufR[i];
        }
    }

    // ===== 参数控制 =====
    void setRoomsize(float value) {
        if (value < 0.0f) value = 0.0f;
        if (value > 1.0f) value = 1.0f;
        combFeedback = static_cast<int>(value * 9175.04f) + 22937;
    }

    void setDamping(float value) {
        if (value < 0.0f) value = 0.0f;
        if (value > 1.0f) value = 1.0f;
        combDamp1 = static_cast<int>(value * 13107.2f);
        combDamp2 = 32768 - combDamp1;
    }

    void setWetLevel(float value) {
        if (value < 0.0f) value = 0.0f;
        if (value > 1.0f) value = 1.0f;
        wetScale = static_cast<int>(value * 30);  // 原代码最终乘以30
    }

    void setDryLevel(float value) {
        // 干声直接通过，暂不处理
        // 原代码中干声是直通的
    }

    // ===== 核心处理函数（立体声 int16_t 输入/输出） =====
    void process(const int16_t* inL, const int16_t* inR,
                 int16_t* outL, int16_t* outR,
                 int numSamples) {
        for (int i = 0; i < numSamples; ++i) {
            // 输入缩放（原代码：*8738 >> 17，约 0.1333 倍）
            int16_t inputL = sat16(static_cast<int32_t>(inL[i]) * 8738, 17);
            int16_t inputR = sat16(static_cast<int32_t>(inR[i]) * 8738, 17);

            // ---- 左声道：8个梳状滤波器 ----
            int32_t sumL = 0;
            for (int c = 0; c < 8; ++c) {
                int16_t bufout = combBufL[c][combIndexL[c]];
                sumL += bufout;
                combFilterL[c] = sat16(
                    static_cast<int32_t>(bufout) * combDamp2 +
                    static_cast<int32_t>(combFilterL[c]) * combDamp1,
                    15
                );
                combBufL[c][combIndexL[c]] = sat16(
                    static_cast<int32_t>(inputL) +
                    sat16(static_cast<int32_t>(combFilterL[c]) * combFeedback, 15),
                    0
                );
                if (++combIndexL[c] >= combLengths[c]) combIndexL[c] = 0;
            }
            int16_t outputL = sat16(sumL * 31457, 17);

            // ---- 右声道：8个梳状滤波器 ----
            int32_t sumR = 0;
            for (int c = 0; c < 8; ++c) {
                int16_t bufout = combBufR[c][combIndexR[c]];
                sumR += bufout;
                combFilterR[c] = sat16(
                    static_cast<int32_t>(bufout) * combDamp2 +
                    static_cast<int32_t>(combFilterR[c]) * combDamp1,
                    15
                );
                combBufR[c][combIndexR[c]] = sat16(
                    static_cast<int32_t>(inputR) +
                    sat16(static_cast<int32_t>(combFilterR[c]) * combFeedback, 15),
                    0
                );
                if (++combIndexR[c] >= combLengths[c]) combIndexR[c] = 0;
            }
            int16_t outputR = sat16(sumR * 31457, 17);

            // ---- 左声道：4个全通滤波器 ----
            for (int a = 0; a < 4; ++a) {
                int16_t bufout = allpassBufL[a][allpassIndexL[a]];
                allpassBufL[a][allpassIndexL[a]] = outputL + (bufout >> 1);
                outputL = sat16(static_cast<int32_t>(bufout) - outputL, 1);
                if (++allpassIndexL[a] >= allpassLengths[a]) allpassIndexL[a] = 0;
            }

            // ---- 右声道：4个全通滤波器 ----
            for (int a = 0; a < 4; ++a) {
                int16_t bufout = allpassBufR[a][allpassIndexR[a]];
                allpassBufR[a][allpassIndexR[a]] = outputR + (bufout >> 1);
                outputR = sat16(static_cast<int32_t>(bufout) - outputR, 1);
                if (++allpassIndexR[a] >= allpassLengths[a]) allpassIndexR[a] = 0;
            }

            // 最终输出（乘以 wetScale，原代码中 wetScale=30）
            outL[i] = sat16(static_cast<int32_t>(outputL) * wetScale, 0);
            outR[i] = sat16(static_cast<int32_t>(outputR) * wetScale, 0);
        }
    }

private:
    float sampleRate;

    // 延迟线长度（根据采样率动态计算）
    int combLengths[8];
    int allpassLengths[4];

    // 梳状滤波器缓冲区（左右各8个）
    int16_t* combBufL[8];
    int16_t* combBufR[8];
    int combIndexL[8];
    int combIndexR[8];
    int16_t combFilterL[8];
    int16_t combFilterR[8];

    // 全通滤波器缓冲区（左右各4个）
    int16_t* allpassBufL[4];
    int16_t* allpassBufR[4];
    int allpassIndexL[4];
    int allpassIndexR[4];

    // 参数
    int combDamp1 = 6553;      // 默认阻尼系数1
    int combDamp2 = 26215;     // 默认阻尼系数2
    int combFeedback = 27524;  // 默认反馈系数
    int wetScale = 30;         // 湿声输出缩放
};

#endif