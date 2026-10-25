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

#ifndef MDA_COMBO_H
#define MDA_COMBO_H

#include "audioeffectx.h"

#include <cstdint>

class mdaCombo : public AudioEffectX
{
public:
	explicit mdaCombo(audioMasterCallback audioMaster);
	~mdaCombo() override;

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

	float filterFreq(float hz);
	void suspend() override;

	bool getEffectName(char *name) override;
	bool getVendorString(char *text) override;
	bool getProductString(char *text) override;
	int32_t getVendorVersion() override { return 1000; }

protected:
	float fParam1;
  float fParam2;
  float fParam3;
  float fParam4;
  float fParam5;
  float fParam6;
  float fParam7;

  float clip, drive, trim, lpf, hpf, mix1, mix2;
  float ff1, ff2, ff3, ff4, ff5, bias;
  float ff6, ff7, ff8, ff9, ff10;
  float hhf, hhq, hh0, hh1; //hpf

  float *buffer, *buffe2;
	int32_t size, bufpos, del1, del2;
  int mode, ster;

	char programName[32];
};

#endif
