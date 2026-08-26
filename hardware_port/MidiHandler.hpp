// MidiHandler.hpp - 类似 libDaisy MidiHandler，带事件队列
#ifndef MIDI_HANDLER_HPP
#define MIDI_HANDLER_HPP

#include "MidiEvent.hpp"
#include "MidiParser.hpp"
#include <queue>

namespace daisy {

/** @brief Simple MIDI handler with a FIFO queue for events */
class MidiHandler {
public:
    MidiHandler() : parser_() {}
    ~MidiHandler() {}

    /** Initialize parser */
    void Init() { parser_.Init(); }

    /** Feed one byte into the parser; if complete event is parsed, enqueue it */
    void PushByte(uint8_t byte) {
        MidiEvent ev;
        if (parser_.Parse(byte, &ev)) {
            if (queue_.size() < kMaxQueueSize) {
                queue_.push(ev);
            }
            // 如果队列满，丢弃新事件
        }
    }

    /** Check if there are events waiting */
    bool HasEvents() const { return !queue_.empty(); }

    /** Pop the oldest event from the queue */
    MidiEvent PopEvent() {
        MidiEvent ev = queue_.front();
        queue_.pop();
        return ev;
    }

    /** Reset parser and clear queue */
    void Reset() {
        parser_.Reset();
        while (!queue_.empty()) queue_.pop();
    }

private:
    static constexpr size_t kMaxQueueSize = 128;
    MidiParser parser_;
    std::queue<MidiEvent> queue_;
};

} // namespace daisy

#endif