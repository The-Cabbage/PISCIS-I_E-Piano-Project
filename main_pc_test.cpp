// main_pc_test.cpp - 测试完整 MDA 引擎，生成单音 WAV
#include <iostream>
#include <memory>
#include <cstring>
#include <cmath>
#include <fstream>
#include "MdaEPianoEngine.h"
#include "config.h"

// ==================== WAV 文件写入函数 ====================
void write_wav(const char* filename, int16_t* dataL, int16_t* dataR, uint32_t numSamples, int sampleRate = 44100) {
    std::ofstream f(filename, std::ios::binary);
    if (!f) {
        std::cerr << "❌ 无法创建 WAV 文件" << std::endl;
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
    uint16_t bitsPerSample = 16;
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

// ==================== 主函数 ====================
int main() {
    std::cout << "🚀 测试完整 MDA 引擎：单音 C2 (音符 36)，持续 3 秒" << std::endl;

    auto engine = std::make_unique<MdaEPianoEngine>();
    engine->setVolume(0.8f);
    engine->noteOn(36, 100);   // 触发 C2

    const int sampleRate = 44100;
    const int blockSize = 256;          // 与 config.h 一致
    const int totalSeconds = 3;
    const uint32_t totalSamples = sampleRate * totalSeconds;

    int16_t* outL = new int16_t[totalSamples];
    int16_t* outR = new int16_t[totalSamples];
    int16_t tempL[blockSize];
    int16_t tempR[blockSize];

    std::cout << "▶️ 开始渲染..." << std::endl;
    uint32_t sampleIndex = 0;
    int renderCount = 0;

    while (sampleIndex < totalSamples) {
        engine->render(tempL, tempR, blockSize);

        for (int i = 0; i < blockSize && sampleIndex < totalSamples; i++) {
            outL[sampleIndex] = tempL[i];
            outR[sampleIndex] = tempR[i];
            sampleIndex++;
        }

        renderCount++;
        if (renderCount % 500 == 0) {
            std::cout << "   已渲染 " << renderCount << " 块 (" 
                      << (sampleIndex * 100 / totalSamples) << "%)" << std::endl;
        }
    }

    write_wav("single_note.wav", outL, outR, sampleIndex, sampleRate);
    delete[] outL;
    delete[] outR;
    std::cout << "🎵 完成！播放 single_note.wav" << std::endl;
    return 0;
}