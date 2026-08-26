// MidiParser.hpp
#ifndef MIDI_PARSER_HPP
#define MIDI_PARSER_HPP

#include "MidiEvent.hpp"
#include <cstdint>

namespace daisy {

/** @brief Utility class for parsing raw byte streams into MIDI messages */
class MidiParser {
public:
    MidiParser();
    ~MidiParser();

    /** Initialize parser to default state */
    void Init();

    /**
     * @brief Parse one MIDI byte.
     * @param byte Raw MIDI byte
     * @param event_out Pointer to output event, filled if parse completes
     * @return true if a complete event was parsed and event_out is valid
     */
    bool Parse(uint8_t byte, MidiEvent* event_out);

    /** Reset the parser to initial state (discard any partial message) */
    void Reset();

private:
    enum ParserState {
        ParserEmpty,        // 等待状态字节
        ParserHasStatus,    // 已有状态字节，等待第一个数据
        ParserHasData0,     // 已有第一个数据，等待第二个
        ParserSysEx,        // 正在接收 SysEx（本实现中会跳过）
    };

    ParserState state_;
    uint8_t running_status_;
    MidiEvent incoming_event_;
    uint8_t data_count_;   // 已经收到的数据字节数
};

} // namespace daisy

#endif