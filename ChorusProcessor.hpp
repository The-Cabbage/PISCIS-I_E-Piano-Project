// ChorusProcessor.hpp
// 高品质立体声合唱处理器，基于反馈延迟线
#ifndef CHORUS_PROCESSOR_HPP
#define CHORUS_PROCESSOR_HPP

#include <cstdint>
#include <cmath>
#include <cstring>
#include <algorithm>

#ifndef PI_F
#define PI_F 3.14159265358979323846f
#endif

class ChorusProcessor {
public:
    /**
     * @param sampleRate 采样率（Hz）
     */
    ChorusProcessor(float sampleRate = 44100.0f)
        : sampleRate(sampleRate)
    {
        // 最大延迟时间 60ms（合唱常用范围 15~45ms）
        maxDelaySamples = static_cast<int>(60.0f * sampleRate / 1000.0f);
        if (maxDelaySamples < 100) maxDelaySamples = 100;

        // 分配延迟线（左 + 右）
        delayLineL = new float[maxDelaySamples];
        delayLineR = new float[maxDelaySamples];
        std::memset(delayLineL, 0, maxDelaySamples * sizeof(float));
        std::memset(delayLineR, 0, maxDelaySamples * sizeof(float));

        writeIndexL = 0;
        writeIndexR = 0;

        // 默认参数（温暖合唱）
        setDelayMs(25.0f);
        setModulationRate(1.2f);
        setModulationDepth(0.6f);
        setFeedback(0.25f);
        setMix(0.4f);
        setWaveform(1); // 三角波

        lfoPhase = 0.0f;
    }

    ~ChorusProcessor() {
        delete[] delayLineL;
        delete[] delayLineR;
    }

    // ===== 参数设置 =====
    void setDelayMs(float ms) {
        if (ms < 5.0f) ms = 5.0f;
        if (ms > 45.0f) ms = 45.0f;
        baseDelaySamples = static_cast<int>(ms * sampleRate / 1000.0f);
        if (baseDelaySamples >= maxDelaySamples) baseDelaySamples = maxDelaySamples - 10;
    }

    void setModulationRate(float rateHz) {
        if (rateHz < 0.1f) rateHz = 0.1f;
        if (rateHz > 10.0f) rateHz = 10.0f;
        lfoDelta = 2.0f * PI_F * rateHz / sampleRate;
    }

    void setModulationDepth(float depth) {
        if (depth < 0.0f) depth = 0.0f;
        if (depth > 1.0f) depth = 1.0f;
        modDepth = depth * 0.5f; // 最大偏移 50% 的基准延迟
    }

    void setFeedback(float fb) {
        if (fb < 0.0f) fb = 0.0f;
        if (fb > 0.9f) fb = 0.9f; // 限制反馈避免自激
        feedback = fb;
    }

    void setMix(float wet) {
        if (wet < 0.0f) wet = 0.0f;
        if (wet > 1.0f) wet = 1.0f;
        mixWet = wet;
        mixDry = 1.0f - wet * 0.7f; // 干声稍微衰减，保持整体音量
    }

    void setWaveform(int type) {
        waveformType = type; // 0=正弦波，1=三角波
    }

    // ===== 核心处理 =====
    void process(const int16_t* inL, const int16_t* inR,
                 int16_t* outL, int16_t* outR,
                 uint32_t numSamples) {
        for (uint32_t i = 0; i < numSamples; ++i) {
            // 更新 LFO
            lfoPhase += lfoDelta;
            if (lfoPhase >= 2.0f * PI_F) lfoPhase -= 2.0f * PI_F;

            // 生成调制信号（左声道 + 右声道反相）
            float lfoL, lfoR;
            if (waveformType == 0) {
                // 正弦波
                float sinVal = std::sinf(lfoPhase);
                lfoL = sinVal * 0.5f + 0.5f;
                lfoR = (-sinVal) * 0.5f + 0.5f;
            } else {
                // 三角波（更自然的合唱音色）
                float tri = (lfoPhase / (2.0f * PI_F)) * 4.0f;
                if (tri >= 3.0f) tri = 4.0f - tri;
                else if (tri >= 1.0f) tri = 2.0f - tri;
                lfoL = tri * 0.5f + 0.25f;
                lfoR = (1.0f - tri) * 0.5f + 0.25f;
            }

            // 计算调制偏移量（样本数）
            float maxOffset = baseDelaySamples * modDepth;
            float offsetL = lfoL * maxOffset;
            float offsetR = lfoR * maxOffset;

            // 左声道：读 + 写
            float inputL = static_cast<float>(inL[i]) / 32768.0f;

            // 读取延迟（带线性插值）
            float readPosL = static_cast<float>(writeIndexL) - (static_cast<float>(baseDelaySamples) + offsetL);
            if (readPosL < 0) readPosL += maxDelaySamples;
            int readIdxL = static_cast<int>(readPosL);
            float fracL = readPosL - readIdxL;
            int nextIdxL = (readIdxL + 1) % maxDelaySamples;
            float delayedL = delayLineL[readIdxL] * (1.0f - fracL) + delayLineL[nextIdxL] * fracL;

            // 写入延迟线（带反馈）
            delayLineL[writeIndexL] = inputL + feedback * delayedL;
            writeIndexL = (writeIndexL + 1) % maxDelaySamples;

            // 右声道
            float inputR = static_cast<float>(inR[i]) / 32768.0f;
            float readPosR = static_cast<float>(writeIndexR) - (static_cast<float>(baseDelaySamples) + offsetR);
            if (readPosR < 0) readPosR += maxDelaySamples;
            int readIdxR = static_cast<int>(readPosR);
            float fracR = readPosR - readIdxR;
            int nextIdxR = (readIdxR + 1) % maxDelaySamples;
            float delayedR = delayLineR[readIdxR] * (1.0f - fracR) + delayLineR[nextIdxR] * fracR;
            delayLineR[writeIndexR] = inputR + feedback * delayedR;
            writeIndexR = (writeIndexR + 1) % maxDelaySamples;

            // 混合干湿
            float outLf = inputL * mixDry + delayedL * mixWet;
            float outRf = inputR * mixDry + delayedR * mixWet;

            // 限幅
            if (outLf > 1.0f) outLf = 1.0f;
            if (outLf < -1.0f) outLf = -1.0f;
            if (outRf > 1.0f) outRf = 1.0f;
            if (outRf < -1.0f) outRf = -1.0f;

            outL[i] = static_cast<int16_t>(outLf * 32767.0f);
            outR[i] = static_cast<int16_t>(outRf * 32767.0f);
        }
    }

private:
    float sampleRate;
    int maxDelaySamples;
    float* delayLineL;
    float* delayLineR;
    int writeIndexL, writeIndexR;

    int baseDelaySamples;
    float modDepth;
    float lfoDelta;
    float lfoPhase;
    float feedback;
    float mixWet, mixDry;
    int waveformType; // 0=正弦，1=三角
};

#endif