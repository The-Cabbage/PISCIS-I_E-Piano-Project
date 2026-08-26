// MidiParser.cpp
#include "MidiParser.hpp"
#include <cstddef>

namespace daisy {

MidiParser::MidiParser() {
    Init();
}

MidiParser::~MidiParser() {}

void MidiParser::Init() {
    Reset();
}

void MidiParser::Reset() {
    state_ = ParserEmpty;
    running_status_ = 0;
    data_count_ = 0;
    incoming_event_.type = MessageLast;
    incoming_event_.channel = 0;
    incoming_event_.data[0] = 0;
    incoming_event_.data[1] = 0;
}

bool MidiParser::Parse(uint8_t byte, MidiEvent* event_out) {
    if (!event_out) return false;

    // 1. 处理系统实时消息（优先级最高，可插入任意位置）
    if (byte >= 0xF8 && byte <= 0xFF) {
        // 实时消息不改变运行状态，直接生成事件
        incoming_event_.type = SystemRealTime;
        incoming_event_.channel = 0;
        incoming_event_.data[0] = byte;
        incoming_event_.data[1] = 0;
        *event_out = incoming_event_;
        return true;
    }

    // 2. 处理 SysEx 开始/结束（本实现简单跳过数据）
    if (byte == 0xF0) {
        state_ = ParserSysEx;
        return false;
    }
    if (byte == 0xF7) {
        state_ = ParserEmpty;
        return false;
    }
    if (state_ == ParserSysEx) {
        // 忽略 SysEx 内部数据
        return false;
    }

    // 3. 处理状态字节（0x80 - 0xEF）
    if (byte & 0x80) {
        running_status_ = byte;
        state_ = ParserHasStatus;
        data_count_ = 0;
        return false;
    }

    // 4. 数据字节（0x00 - 0x7F）
    if (state_ == ParserHasStatus || state_ == ParserHasData0) {
        // 保存数据
        incoming_event_.data[data_count_++] = byte;

        // 判断消息长度
        uint8_t cmd = running_status_ & 0xF0;
        uint8_t expected_len = 0;
        if (cmd == 0xC0 || cmd == 0xD0) {
            expected_len = 1;   // Program Change, Channel Pressure
        } else if (cmd >= 0x80 && cmd <= 0xE0) {
            expected_len = 2;   // Note, CC, PitchBend, etc.
        } else {
            // 未知命令，重置
            Reset();
            return false;
        }

        if (data_count_ == expected_len) {
            // 消息完整，组装事件
            uint8_t type_byte = running_status_ & 0xF0;
            switch (type_byte) {
                case 0x80: incoming_event_.type = NoteOff; break;
                case 0x90: 
                    incoming_event_.type = (incoming_event_.data[1] == 0) ? NoteOff : NoteOn;
                    break;
                case 0xA0: incoming_event_.type = PolyphonicKeyPressure; break;
                case 0xB0: 
                    // 检查是否是通道模式消息
                    if (incoming_event_.data[0] >= 120 && incoming_event_.data[0] <= 127)
                        incoming_event_.type = ChannelMode;
                    else
                        incoming_event_.type = ControlChange;
                    break;
                case 0xC0: incoming_event_.type = ProgramChange; break;
                case 0xD0: incoming_event_.type = ChannelPressure; break;
                case 0xE0: incoming_event_.type = PitchBend; break;
                default:   incoming_event_.type = MessageLast; break;
            }
            incoming_event_.channel = running_status_ & 0x0F;
            // 对于短消息，data[1] 置零
            if (expected_len == 1) incoming_event_.data[1] = 0;

            *event_out = incoming_event_;

            // 重置状态（保持运行状态，以便下次继续）
            state_ = ParserHasStatus;  // 等待下一个数据字节
            data_count_ = 0;
            return true;
        } else {
            // 还没收完，进入等待第二个数据字节状态
            state_ = ParserHasData0;
            return false;
        }
    }

    // 无效状态，重置
    Reset();
    return false;
}

} // namespace daisy