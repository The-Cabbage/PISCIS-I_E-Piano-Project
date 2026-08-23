// AudioEngineFactory.h
#ifndef AUDIO_ENGINE_FACTORY_H
#define AUDIO_ENGINE_FACTORY_H

#include "AudioEngineInterface.h"
#include "MdaEPianoEngine.h"
#include "FluidSynthEngine.h"   // 新增
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

/**
 * @brief 音源引擎工厂
 * 
 * 根据类型字符串创建对应的引擎实例。
 * 支持运行时动态切换音源，上层代码无需改动。
 */
class AudioEngineFactory {
public:
    // 获取单例实例
    static AudioEngineFactory& instance() {
        static AudioEngineFactory factory;
        return factory;
    }

    // 注册引擎类型（用于扩展）
    void registerEngine(const std::string& type, std::function<std::unique_ptr<AudioEngineInterface>()> creator) {
        creators[type] = creator;
    }

    // 创建引擎实例
    std::unique_ptr<AudioEngineInterface> create(const std::string& type) {
        auto it = creators.find(type);
        if (it != creators.end()) {
            return it->second();
        }
        return nullptr;
    }

private:
    AudioEngineFactory() {
        // 默认注册 MDA EPiano 引擎
        registerEngine("mda", []() {
            return std::make_unique<MdaEPianoEngine>();
        });

        // 注册 FluidSynth 多乐器引擎
        registerEngine("fluidsynth", []() {
            // 注意：路径根据你的实际文件位置调整
            return std::make_unique<FluidSynthEngine>(
                "soundfont/FluidR3_GM.sf2",  // SoundFont 路径
                "railgun.mid",               // MIDI 文件路径
                44100,                       // 采样率
                256,                         // 块大小
                0.8f                         // 音量增益
            );
        });
    }

    std::unordered_map<std::string, std::function<std::unique_ptr<AudioEngineInterface>()>> creators;
};

#endif