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

#ifndef MDA_MULTI_BAND_H
#define MDA_MULTI_BAND_H

#include "audioeffectx.h"

#include <cstdint>

class mdaMultiBand : public AudioEffectX
{
public:
	explicit mdaMultiBand(audioMasterCallback audioMaster);
	~mdaMultiBand() override;

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

	bool getEffectName(char *name) override;
	bool getVendorString(char *text) override;
	bool getProductString(char *text) override;
	int32_t getVendorVersion() override { return 1000; }

protected:
	float fParam1, fParam2, fParam3, fParam4;
	float fParam5, fParam6, fParam7, fParam8;
  float fParam9, fParam10, fParam11, fParam12, fParam13;
  float gain1, driv1, att1, rel1, trim1;
  float gain2, driv2, att2, rel2, trim2;
  float gain3, driv3, att3, rel3, trim3;
  float fi1, fb1, fo1, fi2, fb2, fo2, fb3, slev;
  int mswap;

	char programName[32];
};

#endif
