/*
  Copyright 2008-2026 David Robillard <d@drobilla.net>
  Copyright 1999-2000 Paul Kellett (Maxim Digital Audio)

  This is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this software. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MDA_PIANO_H
#define MDA_PIANO_H

#include "audioeffectx.h"

#include <lv2/atom/atom.h>

#include <cstdint>

#define NPARAMS 12       //number of parameters
#define NPROGS   8       //number of programs
#define NOUTS    2       //number of outputs
#define NVOICES 32       //max polyphony
#define SUSTAIN 128
#define SILENCE 0.0001f  //voice choking
#define WAVELEN 586348   //wave data bytes

class mdaPianoProgram
{
  friend class mdaPiano;
public:
	mdaPianoProgram();

private:
  float param[NPARAMS];
  char  name[24];
};


struct VOICE  //voice state
{
  int32_t  delta;  //sample playback
  int32_t  frac;
  int32_t  pos;
  int32_t  end;
  int32_t  loop;

  float env;  //envelope
  float dec;

  float f0;   //first-order LPF
  float f1;
  float ff;

  float outl;
  float outr;
  int32_t  note; //remember what note triggered this
};


struct KGRP  //keygroup
{
  int32_t  root;  //MIDI root note
  int32_t  high;  //highest note
  int32_t  pos;
  int32_t  end;
  int32_t  loop;
};

class mdaPiano : public AudioEffectX
{
public:
	explicit mdaPiano(audioMasterCallback audioMaster);
	~mdaPiano() override;

	void processReplacing(const float **inputs, float **outputs, int32_t sampleframes) override;

	void setProgram(int32_t program) override;
	void setProgramName(const char *name) override;
	void getProgramName(char *name) override;
	void setParameter(int32_t index, float value) override;
	float getParameter(int32_t index) override;
	void getParameterLabel(int32_t index, char *label) override;
	void getParameterDisplay(int32_t index, char *text) override;
	void getParameterName(int32_t index, char *text) override;
	void setSampleRate(float sampleRate) override;
	void setBlockSize(int32_t blockSize) override;
  void resume() override;

	bool getOutputProperties (int32_t index, LvzPinProperties* properties);
	bool getProgramNameIndexed (int32_t category, int32_t index, char* text) override;
	bool copyProgram (int32_t destination);
	bool getEffectName (char* name) override;
	bool getVendorString (char* text) override;
	bool getProductString (char* text) override;
	int32_t getVendorVersion () override {return 1;}
	int32_t canDo (const char* text) override;

  virtual int32_t getNumMidiInputChannels ()  { return 1; }

  int32_t guiUpdate;
  void guiGetDisplay(int32_t index, char *label);

private:
	int32_t processEvent(const LV2_Atom_Event* ev);
	void update();  //my parameter update
  void noteOn(int32_t note, int32_t velocity);
  void fillpatch(int32_t p, const char *name, float p0, float p1, float p2, float p3, float p4,
                 float p5, float p6, float p7, float p8, float p9, float p10,float p11);

  mdaPianoProgram* programs;
  float Fs, iFs;

  ///global internal variables
  KGRP  kgrp[16];
  VOICE voice[NVOICES];
  int32_t  activevoices, poly, cpos;
  const short *waves;
  int32_t  cmax;
  float *comb, cdep, width, trim;
  int32_t  size, sustain;
  float fine, random, stretch;
  float muff, muffvel, sizevel, velsens, volume;
};

#endif
