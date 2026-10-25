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

#ifndef MDA_BEAT_BOX_H
#define MDA_BEAT_BOX_H

#include "audioeffectx.h"

#include <cstdint>
#include <random>

class mdaBeatBox : public AudioEffectX
{
public:
	explicit mdaBeatBox(audioMasterCallback audioMaster);
	~mdaBeatBox() override;

	void process(const float **inputs, float **outputs, int32_t sampleFrames) override;
	void processReplacing(const float **inputs, float **outputs, int32_t sampleFrames) override;
	void setProgramName(const char *name) override;
	void getProgramName(char *name) override;
	bool getProgramNameIndexed (int32_t category, int32_t index, char* name) override;
	void setParameter(int32_t index, float value) override;
	float getParameter(int32_t index) override;
	void getParameterLabel(int32_t index, char *label) override;
	void getParameterDisplay(int32_t index, char *text) override;
	void getParameterName(int32_t index, char *text) override;
	void suspend() override;
  void synth();

	bool getEffectName(char *name) override;
	bool getVendorString(char *text) override;
	bool getProductString(char *text) override;
	int32_t getVendorVersion() override { return 1000; }

protected:
  std::minstd_rand rng;
  std::uniform_real_distribution<float> dist;

	float fParam1;
  float fParam2;
  float fParam3;
  float fParam4;
  float fParam5;
  float fParam6;
  float fParam7;
	float fParam8;
  float fParam9;
  float fParam10;
  float fParam11;
  float fParam12;
  float hthr, hfil, sthr, kthr, mix;
  float klev, hlev, slev;
  float  ww,  wwx,  sb1,  sb2,  sf1,  sf2, sf3;
  float kww, kwwx, ksb1, ksb2, ksf1, ksf2;
  float dyne, dyna, dynr, dynm;

  float *hbuf;
  float *kbuf;
  float *sbuf, *sbuf2;
	int32_t hbuflen, hbufpos, hdel;
	int32_t sbuflen, sbufpos, sdel, sfx;
  int32_t kbuflen, kbufpos, kdel, ksfx;
  int32_t rec, recx, recpos;

	char programName[32];
};

#endif
