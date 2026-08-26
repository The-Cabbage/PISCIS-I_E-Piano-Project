// MidiEvent.hpp
#ifndef MIDI_EVENT_HPP
#define MIDI_EVENT_HPP

#include <cstdint>

namespace daisy {

// ---- 消息类型枚举 ----
enum MidiMessageType {
    NoteOff,                // 0x80
    NoteOn,                 // 0x90
    PolyphonicKeyPressure,  // 0xA0
    ControlChange,          // 0xB0
    ProgramChange,          // 0xC0
    ChannelPressure,        // 0xD0
    PitchBend,              // 0xE0
    SystemCommon,           // 0xF0
    SystemRealTime,         // 0xF8
    ChannelMode,            // 0xB0 特殊子集
    MessageLast,
};

// ---- 系统公共消息子类型 ----
enum SystemCommonType {
    SystemExclusive = 0xF0,
    MTCQuarterFrame = 0xF1,
    SongPositionPointer = 0xF2,
    SongSelect = 0xF3,
    SCUndefined0 = 0xF4,
    SCUndefined1 = 0xF5,
    TuneRequest = 0xF6,
    SysExEnd = 0xF7,
};

// ---- 系统实时消息子类型 ----
enum SystemRealTimeType {
    TimingClock = 0xF8,
    SRTUndefined0 = 0xF9,
    Start = 0xFA,
    Continue = 0xFB,
    Stop = 0xFC,
    SRTUndefined1 = 0xFD,
    ActiveSensing = 0xFE,
    Reset = 0xFF,
};

// ---- 通道模式子类型（ControlChange 的特殊值） ----
enum ChannelModeType {
    AllSoundOff = 120,
    ResetAllControllers = 121,
    LocalControl = 122,
    AllNotesOff = 123,
    OmniModeOff = 124,
    OmniModeOn = 125,
    MonoModeOn = 126,
    PolyModeOn = 127,
    ChannelModeLast,
};

// ---- 具体事件结构体 ----
struct NoteOffEvent {
    int channel;
    uint8_t note;
    uint8_t velocity;
};

struct NoteOnEvent {
    int channel;
    uint8_t note;
    uint8_t velocity;
};

struct PolyphonicKeyPressureEvent {
    int channel;
    uint8_t note;
    uint8_t pressure;
};

struct ControlChangeEvent {
    int channel;
    uint8_t control_number;
    uint8_t value;
};

struct ProgramChangeEvent {
    int channel;
    uint8_t program;
};

struct ChannelPressureEvent {
    int channel;
    uint8_t pressure;
};

struct PitchBendEvent {
    int channel;
    int16_t value;   // 14-bit 值，0x2000 为中心
};

struct ChannelModeEvent {
    int channel;
    ChannelModeType event_type;
    int16_t value;
};

// ---- 通用 MidiEvent（解析器直接产出） ----
struct MidiEvent {
    MidiMessageType type;
    uint8_t channel;
    uint8_t data[2];      // 两个数据字节

    // 转换为具体事件的辅助方法
    NoteOffEvent AsNoteOff() const;
    NoteOnEvent AsNoteOn() const;
    ControlChangeEvent AsControlChange() const;
    ProgramChangeEvent AsProgramChange() const;
    PitchBendEvent AsPitchBend() const;
    ChannelModeEvent AsChannelMode() const;
    // ... 其他可按需添加
};

// ---- 实现（inline） ----
inline NoteOffEvent MidiEvent::AsNoteOff() const {
    return {channel, data[0], data[1]};
}
inline NoteOnEvent MidiEvent::AsNoteOn() const {
    return {channel, data[0], data[1]};
}
inline ControlChangeEvent MidiEvent::AsControlChange() const {
    return {channel, data[0], data[1]};
}
inline ProgramChangeEvent MidiEvent::AsProgramChange() const {
    return {channel, data[0]};
}
inline PitchBendEvent MidiEvent::AsPitchBend() const {
    int16_t val = (data[1] << 7) | data[0];
    return {channel, val};
}
inline ChannelModeEvent MidiEvent::AsChannelMode() const {
    return {channel, static_cast<ChannelModeType>(data[0]), data[1]};
}

} // namespace daisy

#endif