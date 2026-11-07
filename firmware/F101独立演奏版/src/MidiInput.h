#pragma once

#include <stdint.h>
#include <string.h>
#include "DualSynth.h"

extern "C" {
#include "midi_host.h"
}

// Merge the keyboard's cables and channels into one instrument. Keep note
// ownership per source so releasing a key on one channel cannot silence
// the same pitch while another channel still holds it.
class MidiInput {
    static constexpr unsigned channel_count = 16;
    static constexpr unsigned cable_count = 16;
    static constexpr unsigned note_count = 128;

    struct Source {
        bool held[note_count];
        bool sustain;
    };

    DualSynth &synth;
    Source sources[cable_count][channel_count]{};
    uint16_t note_holders[note_count]{};
    unsigned sustain_sources = 0;

    void clear()
    {
        memset(sources, 0, sizeof(sources));
        memset(note_holders, 0, sizeof(note_holders));
        sustain_sources = 0;
    }

    void release(Source &source, unsigned note)
    {
        if (!source.held[note]) return;
        source.held[note] = false;
        if (--note_holders[note] == 0) synth.noteOn(note, 0);
    }

    void set_sustain(Source &source, bool pressed)
    {
        if (source.sustain == pressed) return;
        const bool was_sustained = sustain_sources != 0;
        source.sustain = pressed;
        if (pressed) {
            ++sustain_sources;
        } else {
            --sustain_sources;
        }
        if (was_sustained != (sustain_sources != 0)) synth.sustain(sustain_sources != 0);
    }

public:
    explicit MidiInput(DualSynth &engine) : synth(engine) {}

    void reset()
    {
        synth.reset();
        clear();
    }

    void apply(const midi_event &event)
    {
        if (event.cable >= cable_count || event.channel >= channel_count ||
            event.key >= note_count) return;
        Source &source = sources[event.cable][event.channel];
        switch (event.type) {
        case MIDI_NOTE_ON:
            if (!source.held[event.key]) {
                source.held[event.key] = true;
                ++note_holders[event.key];
            }
            synth.noteOn(event.key, event.value);
            break;
        case MIDI_NOTE_OFF:
            release(source, event.key);
            break;
        case MIDI_SUSTAIN:
            set_sustain(source, event.value >= 64);
            break;
        case MIDI_PROGRAM:
            if (event.value <= 2 && synth.configure(event.value, 0, 100, 100)) clear();
            break;
        case MIDI_ALL_NOTES_OFF:
            if (event.key == 120) {
                reset();
            } else {
                for (unsigned note = 0; note < note_count; ++note) release(source, note);
            }
            break;
        case MIDI_RESET_CONTROLLERS:
            set_sustain(source, false);
            break;
        default:
            break;
        }
    }
};
