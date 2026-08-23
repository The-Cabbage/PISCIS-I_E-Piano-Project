// main.cpp - 使用 FluidSynth 引擎渲染 MIDI 文件
#include "AudioEngineFactory.h"
#include <fstream>
#include <iostream>
#include <cstdint>
#include <memory>
#include <string>

void write_wav(const char* filename, int16_t* dataL, int16_t* dataR, uint32_t numSamples, int sampleRate = 44100) {
    std::ofstream f(filename, std::ios::binary);
    if (!f) {
        std::cerr << "无法创建 WAV 文件" << std::endl;
        return;
    }

    uint32_t bytesPerSample = 2;
    uint32_t numChannels = 2;
    uint32_t sampleRateHZ = sampleRate;
    uint32_t byteRate = sampleRateHZ * numChannels * bytesPerSample;
    uint32_t blockAlign = numChannels * bytesPerSample;
    uint32_t dataSize = numSamples * numChannels * bytesPerSample;
    uint32_t chunkSize = 36 + dataSize;

    f.write("RIFF", 4);
    f.write(reinterpret_cast<const char*>(&chunkSize), 4);
    f.write("WAVE", 4);
    f.write("fmt ", 4);
    uint32_t fmtSize = 16;
    uint16_t audioFormat = 1;
    uint16_t channels = numChannels;
    f.write(reinterpret_cast<const char*>(&fmtSize), 4);
    f.write(reinterpret_cast<const char*>(&audioFormat), 2);
    f.write(reinterpret_cast<const char*>(&channels), 2);
    f.write(reinterpret_cast<const char*>(&sampleRateHZ), 4);
    f.write(reinterpret_cast<const char*>(&byteRate), 4);
    f.write(reinterpret_cast<const char*>(&blockAlign), 2);
    uint16_t bitsPerSample = bytesPerSample * 8;
    f.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    f.write("data", 4);
    f.write(reinterpret_cast<const char*>(&dataSize), 4);

    for (uint32_t i = 0; i < numSamples; ++i) {
        f.write(reinterpret_cast<const char*>(&dataL[i]), 2);
        f.write(reinterpret_cast<const char*>(&dataR[i]), 2);
    }
    f.close();
    std::cout << "✅ WAV 文件已生成: " << filename << std::endl;
}

int main(int argc, char* argv[]) {
    // 默认使用 FluidSynth 引擎
    std::string engineType = "fluidsynth";
    if (argc > 1) {
        engineType = argv[1];
    }

    // 如果用户指定 "mda"，提示不支持 MIDI 文件，自动切换回 fluidsynth
    if (engineType == "mda") {
        std::cout << "⚠️ MDA 引擎不支持 MIDI 文件，自动切换到 FluidSynth 引擎。" << std::endl;
        engineType = "fluidsynth";
    }

    std::cout << "🚀 使用引擎: " << engineType << std::endl;

    auto engine = AudioEngineFactory::instance().create(engineType);
    if (!engine) {
        std::cerr << "❌ 创建引擎失败" << std::endl;
        return 1;
    }

    // 设置音量
    engine->setVolume(0.8f);

    // 渲染音频（FluidSynth 引擎会在内部播放 MIDI）
    const int sampleRate = 44100;
    const int blockSize = 256;
    const int maxSeconds = 240; // 安全上限，实际由 isPlaying() 控制
    const int totalSamples = sampleRate * maxSeconds;
    int16_t* outL = new int16_t[totalSamples];
    int16_t* outR = new int16_t[totalSamples];
    int16_t tempL[blockSize], tempR[blockSize];
    uint32_t sampleIndex = 0;

    std::cout << "▶️ 开始合成..." << std::endl;
    bool playing = true;
    while (playing && sampleIndex < totalSamples) {
        engine->render(tempL, tempR, blockSize);
        for (int s = 0; s < blockSize; ++s) {
            outL[sampleIndex] = tempL[s];
            outR[sampleIndex] = tempR[s];
            ++sampleIndex;
        }
        // 每 5 秒打印进度
        if (sampleIndex % (sampleRate * 5) == 0) {
            std::cout << "   已合成 " << sampleIndex / sampleRate << " 秒" << std::endl;
        }
        // ✅ 检查引擎是否还在播放
        if (!engine->isPlaying()) {
            std::cout << "⏹️ 引擎播放完毕，停止渲染" << std::endl;
            break;
        }
        if (sampleIndex >= totalSamples) break;
    }

    // 如果实际采样数少于 totalSamples，调整
    if (sampleIndex < totalSamples) {
        std::cout << "✅ 合成提前结束，实际采样数: " << sampleIndex << std::endl;
    } else {
        std::cout << "✅ 合成达到最大时长限制" << std::endl;
    }

    write_wav("output.wav", outL, outR, sampleIndex, sampleRate);

    delete[] outL;
    delete[] outR;

    std::cout << "🎵 完成！播放 output.wav" << std::endl;
    return 0;
}