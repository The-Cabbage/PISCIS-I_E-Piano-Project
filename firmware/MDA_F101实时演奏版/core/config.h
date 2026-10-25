#include <cstdint>
/*
   MicroMDAEPiano

   MicroMDAEPiano is a port of the MDA-EPiano sound engine
   (https://sourceforge.net/projects/mda-vst/) for the Teensy-3.5/3.6 with audio shield.

   (c)2019-2020 H. Wirtz <wirtz@parasitstudio.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

//#include "midinotes.h"
//#include <Arduino.h>

// ATTENTION! For better latency you have to redefine AUDIO_BLOCK_SAMPLES from
// 128 to 64 in <ARDUINO-IDE-DIR>/cores/teensy3/AudioStream.h

// If you want to test the system with Linux and withous any keyboard and/or audio equipment, you can do the following:
// 1. In Arduino-IDE enable "Tools->USB-Type->Serial + MIDI + Audio"
// 2. Build the firmware with "MIDI_DEVICE_USB" enabled in config.h.
// 3. Afterconnecting to a Linux system there should be a MIDI an audio device available that is called "MicroMDAEPiano", so you can start the following:
// $ aplaymidi -p 20:0 <MIDI-File> # e.g. test.mid
// $ arecord -f cd -Dhw:1,0 /tmp/bla.wav

//*************************************************************************************************
//* DEVICE SETTINGS
//*************************************************************************************************

// MIDI
#define MIDI_DEVICE_DIN Serial1
#define MIDI_DEVICE_USB 1
#define MIDI_DEVICE_USB_HOST 1

//*************************************************************************************************
//* MIDI SETTINGS
//*************************************************************************************************

#define DEFAULT_MIDI_CHANNEL MIDI_CHANNEL_OMNI
#define MIDI_MERGE_THRU 1

//*************************************************************************************************
//* AUDIO SETTINGS
//*************************************************************************************************

#define AUDIO_MEM 128
#define SAMPLE_RATE AUDIO_SAMPLE_RATE
#define REDUCE_LOUDNESS 0
//#define USE_XFADE_DATA 1

// CHORUS parameters
#define MOD_DELAY_SAMPLE_BUFFER int32_t(TIME_MS2SAMPLES(20.0)) // 20.0 ms delay buffer. 
#define MOD_WAVEFORM WAVEFORM_TRIANGLE // WAVEFORM_SINE WAVEFORM_TRIANGLE WAVEFORM_SAWTOOTH WAVEFORM_SAWTOOTH_REVERSE
#define MOD_FILTER_OUTPUT MOD_LINKWITZ_RILEY_FILTER_OUTPUT // MOD_LINKWITZ_RILEY_FILTER_OUTPUT MOD_BUTTERWORTH_FILTER_OUTPUT MOD_NO_FILTER_OUTPUT
#define MOD_FILTER_CUTOFF_HZ 3000

#define PATCHED_FREEVERB 1

//*************************************************************************************************
//* DEBUG OUTPUT SETTINGS
//*************************************************************************************************

#define SHOW_DEBUG 1
#define SERIAL_SPEED 38400
#define SHOW_XRUN 1
#define SHOW_CPU_LOAD_MSEC 5000
//#define DEBUG_AUDIO 50

//*************************************************************************************************
//* HARDWARE SETTINGS
//*************************************************************************************************

#define USB_AUDIO 1

// Teensy Audio Shield:
/* Values for SGTL5000_LINEOUT_LEVEL
  13: 3.16 Volts p-p
  14: 2.98 Volts p-p
  15: 2.83 Volts p-p
  16: 2.67 Volts p-p
  17: 2.53 Volts p-p
  18: 2.39 Volts p-p
  19: 2.26 Volts p-p
  20: 2.14 Volts p-p
  21: 2.02 Volts p-p
  22: 1.91 Volts p-p
  23: 1.80 Volts p-p
  24: 1.71 Volts p-p
  25: 1.62 Volts p-p
  26: 1.53 Volts p-p
  27: 1.44 Volts p-p
  28: 1.37 Volts p-p
  29: 1.29 Volts p-p  (default)
  30: 1.22 Volts p-p
  31: 1.16 Volts p-p
*/
#define SGTL5000_LINEOUT_LEVEL 29
//#define SDCARD_CS_PIN    10
//#define SDCARD_MOSI_PIN  7
//#define SDCARD_SCK_PIN   14
// Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

// Encoder with button
#define NUM_ENCODER 2
#define ENC_L_PIN_A  3
#define ENC_L_PIN_B  2
#define BUT_L_PIN    4
#define INITIAL_ENC_L_VALUE 0
#define ENC_R_PIN_A  28
#define ENC_R_PIN_B  29
#define BUT_R_PIN    30
#define INITIAL_ENC_R_VALUE 0
#define BUT_DEBOUNCE_MS 20
#define LONG_BUTTON_PRESS 500

// LCD display (I2C)
// [I2C] SCL: Pin 19, SDA: Pin 18 (https://www.pjrc.com/teensy/td_libs_Wire.html)
#define LCD_I2C_ADDRESS 0x27
#define LCD_CHARS 16
#define LCD_LINES 2

// EEPROM address
#define EEPROM_START_ADDRESS 0
#define EEPROM_MASTER_VOLUME EEPROM_START_ADDRESS
#define EEPROM_SOUND EEPROM_START_ADDRESS+1
#define EEPROM_CONFIGURATIONS EEPROM_SOUND+1

// MIDI-CC mapping
// free CCs: 102-119 / 85-90 / 52-63
#define MIDI_CC_PANORAMA 10
#define MIDI_CC_REVERB_SEND 91
#define MIDI_CC_TREMOLO_DEPTH 92
#define MIDI_CC_CHORUS_SEND 93
#define MIDI_CC_DETUNE_DEPTH 94
#define MIDI_CC_EP_DECAY 52
#define MIDI_CC_EP_RELEASE 53
#define MIDI_CC_EP_HARDNESS 54
#define MIDI_CC_EP_TREBLE 55
#define MIDI_CC_EP_STEREO 56
#define MIDI_CC_EP_TRANSPOSE 57
#define MIDI_CC_EP_TUNE 58
#define MIDI_CC_EP_VELOCITY_SENSE 59
#define MIDI_CC_EP_TREM_FRQ 60
#define MIDI_CC_EP_OVERDRIVE 61
#define MIDI_CC_COMP_GAIN 102
#define MIDI_CC_COMP_REPOSNE 103
#define MIDI_CC_COMP_LIMIT 104
#define MIDI_CC_COMP_THRESHOLD 105
#define MIDI_CC_COMP_ATTACK 106
#define MIDI_CC_COMP_DECAY 107
#define MIDI_CC_REVERB_ROOMSIZE 108
#define MIDI_CC_REVERB_DAMPING 109
#define MIDI_CC_CHORUS_FREQUENCY 111
#define MIDI_CC_CHORUS_INTENSITY 112
#define MIDI_CC_CHORUS_WAVEFORM 113
#define MIDI_CC_BASS_LR_LEVEL 114
#define MIDI_CC_BASS_MONO_LEVEL 115
#define MIDI_CC_EQ_BASS 116
#define MIDI_CC_EQ_TREBLE 117
#define MIDI_CC_MIDI_SOFT_THRU 118
#define MIDI_CC_MONO 119

//*************************************************************************************************
//* DO NO CHANGE ANYTHING BEYOND IF YOU DON'T KNOW WHAT YOU ARE DOING !!!
//*************************************************************************************************

#define MICRO_MDAEPIANO_VERSION "1.0.1"

/* HELPER MACROS */
#define TIME_MS2SAMPLES(x) floor(uint32_t(x) * AUDIO_SAMPLE_RATE / 1000)
#define SAMPLES2TIME_MS(x) float(uint32_t(x) * 1000 / AUDIO_SAMPLE_RATE)
#define MOD_NO_FILTER_OUTPUT 0
#define MOD_BUTTERWORTH_FILTER_OUTPUT 1
#define MOD_LINKWITZ_RILEY_FILTER_OUTPUT 2

#define MAX_SOUNDS min(99,int((4096-EEPROM_CONFIGURATIONS)/sizeof(config_t)))

#define CONTROL_RATE_MS 100
#define BACK_TO_MAIN_MS 800
#define STORE_MASTER_VOLUME_MS 5000

// Encoder min/max values
#define ENC_DECAY_MIN 0
#define ENC_DECAY_MAX 99
#define ENC_DECAY_DEFAULT 50
//
#define ENC_RELEASE_MIN 0
#define ENC_RELEASE_MAX 99
#define ENC_RELEASE_DEFAULT 50
//
#define ENC_HARDNESS_MIN 0
#define ENC_HARDNESS_MAX 99
#define ENC_HARDNESS_DEFAULT 50
//
#define ENC_TREBLE_MIN 0
#define ENC_TREBLE_MAX 99
#define ENC_TREBLE_DEFAULT 50
//
#define ENC_STEREO_MIN 0
#define ENC_STEREO_MAX 99
#define ENC_STEREO_DEFAULT 64
//
#define ENC_TRANSPOSE_MIN -24
#define ENC_TRANSPOSE_MAX 24
#define ENC_TRANSPOSE_DEFAULT 0
//
#define ENC_TUNE_MIN -50
#define ENC_TUNE_MAX 50
#define ENC_TUNE_DEFAULT 0
//
#define ENC_DETUNE_MIN 0
#define ENC_DETUNE_MAX 99
#define ENC_DETUNE_DEFAULT 14
//
#define ENC_VELOCITY_SENSE_MIN 0
#define ENC_VELOCITY_SENSE_MAX 99
#define ENC_VELOCITY_SENSE_DEFAULT 24
//
#define ENC_PAN_TREM_FREQUENCY_MIN 0
#define ENC_PAN_TREM_FREQUENCY_MAX 99
#define ENC_PAN_TREM_FREQUENCY_DEFAULT 50
//
#define ENC_PAN_TREM_LEVEL_MIN 0
#define ENC_PAN_TREM_LEVEL_MAX 99
#define ENC_PAN_TREM_LEVEL_DEFAULT 50
//
#define ENC_OVERDRIVE_MIN 0
#define ENC_OVERDRIVE_MAX 99
#define ENC_OVERDRIVE_DEFAULT 15
//
#define ENC_COMP_GAIN_MIN 0
#define ENC_COMP_GAIN_MAX 2
#define ENC_COMP_GAIN_DEFAULT 0
//
#define ENC_COMP_RESPONSE_MIN 0
#define ENC_COMP_RESPONSE_MAX 3
#define ENC_COMP_RESPONSE_DEFAULT 1
//
#define ENC_COMP_LIMIT_MIN 0
#define ENC_COMP_LIMIT_MAX 1
#define ENC_COMP_LIMIT_DEFAULT 1
//
#define ENC_COMP_THRESHOLD_MIN 0
#define ENC_COMP_THRESHOLD_MAX 96
#define ENC_COMP_THRESHOLD_DEFAULT 18
//
#define ENC_COMP_ATTACK_MIN 0
#define ENC_COMP_ATTACK_MAX 99
#define ENC_COMP_ATTACK_DEFAULT 20
//
#define ENC_COMP_DECAY_MIN 0
#define ENC_COMP_DECAY_MAX 99
#define ENC_COMP_DECAY_DEFAULT 40
//
#define ENC_REVERB_ROOMSIZE_MIN 0
#define ENC_REVERB_ROOMSIZE_MAX 99
#define ENC_REVERB_ROOMSIZE_DEFAULT 40
//
#define ENC_REVERB_DAMPING_MIN 0
#define ENC_REVERB_DAMPING_MAX 99
#define ENC_REVERB_DAMPING_DEFAULT 50
//
#define ENC_REVERB_LEVEL_MIN 1
#define ENC_REVERB_LEVEL_MAX 99
#define ENC_REVERB_LEVEL_DEFAULT 15
//
#define ENC_CHORUS_FREQUENCY_MIN 0
#define ENC_CHORUS_FREQUENCY_MAX 200
#define ENC_CHORUS_FREQUENCY_DEFAULT 30
//
#define ENC_CHORUS_INTENSITY_MIN 0
#define ENC_CHORUS_INTENSITY_MAX 100
#define ENC_CHORUS_INTENSITY_DEFAULT 50
//
#define ENC_CHORUS_WAVEFORM_MIN 1
#define ENC_CHORUS_WAVEFORM_MAX 2
#define ENC_CHORUS_WAVEFORM_DEFAULT 1
//
#define ENC_CHORUS_LEVEL_MIN 0
#define ENC_CHORUS_LEVEL_MAX 99
#define ENC_CHORUS_LEVEL_DEFAULT 50
//
#define ENC_BASS_LR_LEVEL_MIN 0
#define ENC_BASS_LR_LEVEL_MAX 99
#define ENC_BASS_LR_LEVEL_DEFAULT 99
//
#define ENC_BASS_MONO_LEVEL_MIN 0
#define ENC_BASS_MONO_LEVEL_MAX 99
#define ENC_BASS_MONO_LEVEL_DEFAULT 0
//
#define ENC_EQ_BASS_MIN -50
#define ENC_EQ_BASS_MAX 50
#define ENC_EQ_BASS_DEFAULT 0
//
#define ENC_EQ_TREBLE_MIN -50
#define ENC_EQ_TREBLE_MAX 50
#define ENC_EQ_TREBLE_DEFAULT 0
//
#define ENC_LOUDNESS_MIN 0
#define ENC_LOUDNESS_MAX 99
#define ENC_LOUDNESS_DEFAULT 99
//
#define ENC_MIDI_CHANNEL_MIN 0
#define ENC_MIDI_CHANNEL_MAX 16
#define ENC_MIDI_CHANNEL_DEFAULT DEFAULT_MIDI_CHANNEL
//
#define ENC_MIDI_SOFT_THRU_MIN 0
#define ENC_MIDI_SOFT_THRU_MAX 1
#define ENC_MIDI_SOFT_THRU_DEFAULT 1
//
#define ENC_MAX_POLY_MIN 1
#define ENC_MAX_POLY_MAX NVOICES
#define ENC_MAX_POLY_DEFAULT NVOICES
//
#define ENC_MONO_MIN 0 // 0=stereo
#define ENC_MONO_MAX 3 // 1=mono(r/l) 2=mono(r) 3=mono(l)
#define ENC_MONO_DEFAULT 0
//
#define ENC_MASTER_VOLUME_MIN 0
#define ENC_MASTER_VOLUME_MAX 99
#define ENC_MASTER_VOLUME_DEFAULT 80
//
#define ENC_MASTER_PAN_MIN -20
#define ENC_MASTER_PAN_MAX 20
#define ENC_MASTER_PAN_DEFAULT 0

// MIDI
#ifdef MIDI_DEVICE_USB
#define USBCON 1
#endif
#if defined(__MK66FX1M0__)
// Teensy-3.6 settings
#define MIDI_DEVICE_USB_HOST 1
#else
// Teensy-3.5 settings
#undef MIDI_DEVICE_USB_HOST
#endif
// Engine
#if defined(__MK66FX1M0__)
// Teensy-3.6 settings
#define NVOICES 40
#else
#define NVOICES 32
#endif

// struct for holding the current configuration
struct config_t {
  uint32_t checksum;
  uint8_t decay;
  uint8_t release;
  uint8_t hardness;
  uint8_t treble;
  int8_t stereo;
  int8_t transpose;
  int8_t tune;
  uint8_t detune;
  uint8_t velocity_sense;
  uint8_t pan_trem_frequency;
  uint8_t pan_trem_level;
  uint8_t overdrive;
  uint8_t comp_gain;
  uint8_t comp_response;
  bool comp_limit;
  uint8_t comp_threshold;
  uint8_t comp_attack;
  uint8_t comp_decay;
  uint8_t reverb_roomsize;
  uint8_t reverb_damping;
  uint8_t reverb_level;
  uint8_t chorus_frequency;
  uint8_t chorus_intensity;
  uint8_t chorus_waveform;
  uint8_t chorus_level;
  uint8_t bass_lr_level;
  uint8_t bass_mono_level;
  int8_t eq_bass;
  int8_t eq_treble;
  uint8_t loudness;
  uint8_t midi_channel;
  bool midi_soft_thru;
  uint8_t max_poly;
  uint8_t mono;
  int8_t pan;
};
// ========== 以下为 PC 测试环境添加的定义 ==========
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_BLOCK_SAMPLES 256
#define ENC_MASTER_VOLUME_MIN 0
#define ENC_MASTER_VOLUME_MAX 99
#define SAMPLE_RATE AUDIO_SAMPLE_RATE   // 有些地方直接用 SAMPLE_RATE
#endif
