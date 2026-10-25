/*
  Copyright 2008-2020 David Robillard <d@drobilla.net>
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

#ifndef MDA_DX10_H
#define MDA_DX10_H

#include "audioeffectx.h"

#include <lv2/atom/atom.h>

#include <cstdint>

#define NPARAMS 16       //number of parameters
#define NPROGS  32       //number of programs
#define NOUTS    2       //number of outputs
#define NVOICES  8       //max polyphony
#define SILENCE 0.0003f  //voice choking

class mdaDX10Program
{
  friend class mdaDX10;
private:
  float param[NPARAMS];
  char  name[24];
};


struct VOICE  //voice state
{
  float env;  //carrier envelope
  float dmod; //modulator oscillator
  float mod0;
  float mod1;
  float menv; //modulator envelope
  float mlev; //modulator target level
  float mdec; //modulator envelope decay
  float car;  //carrier oscillator
  float dcar;
  float cenv; //smoothed env
  float catt; //smoothing
  float cdec; //carrier envelope decay
  int32_t  note; //remember what note triggered this
};


class mdaDX10 : public AudioEffectX
{
public:
	explicit mdaDX10(audioMasterCallback audioMaster);
	~mdaDX10() override;

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

private:
	int32_t processEvent(const LV2_Atom_Event* ev);
	void update();  //my parameter update
  void noteOn(int32_t note, int32_t velocity);
  void fillpatch(int32_t p, const char *name,
                 float p0,  float p1,  float p2,  float p3,  float p4,  float p5,
                 float p6,  float p7,  float p8,  float p9,  float p10, float p11,
                 float p12, float p13, float p14, float p15);

  mdaDX10Program* programs;
  float Fs;

  ///global internal variables
  VOICE voice[NVOICES];
  #define SUSTAIN 128
  int32_t sustain, activevoices, K;

  float tune, rati, ratf, ratio; //modulator ratio
  float catt, cdec, crel;        //carrier envelope
  float depth, dept2, mdec, mrel; //modulator envelope
  float lfo0, lfo1, dlfo, modwhl, MW, pbend, velsens, volume, vibrato; //LFO and CC
  float rich, modmix;
};

#endif
