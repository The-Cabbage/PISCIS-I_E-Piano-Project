#ifndef MIDI_HOST_H
#define MIDI_HOST_H
#include <stdint.h>

enum midi_event_type {
    MIDI_CONNECTED, MIDI_DISCONNECTED, MIDI_NOTE_ON, MIDI_NOTE_OFF,
    MIDI_SUSTAIN, MIDI_RESET, MIDI_ERROR, MIDI_PROGRAM, MIDI_ALL_NOTES_OFF,
    MIDI_RESET_CONTROLLERS, MIDI_VOLUME
};

struct midi_event {
    uint32_t time_ms;
    int32_t detail;
    uint8_t type, cable, channel, key, value;
};

int midi_host_start(void);
void midi_host_poll(void);
/* Called only from thread context; implementation must never block. */
void midi_event_push(const struct midi_event *event);
#endif
