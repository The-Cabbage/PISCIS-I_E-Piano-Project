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

#ifndef MDA_IMAGE_H
#define MDA_IMAGE_H

#include "audioeffectx.h"

#include <cstdint>

class mdaImage : public AudioEffectX
{
public:
	explicit mdaImage(audioMasterCallback audioMaster);
	~mdaImage() override;

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
  float l2l, l2r, r2l, r2r;

  char programName[32];
};

#endif
