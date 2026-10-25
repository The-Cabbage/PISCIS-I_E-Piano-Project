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

#ifndef MDA_SPLITTER_H
#define MDA_SPLITTER_H

#define NPARAMS  7  ///number of parameters
#define NPROGS   3  ///number of programs

#include "audioeffectx.h"

#include <cstdint>
#include <cstring>

class mdaSplitterProgram
{
public:
  mdaSplitterProgram()
  {
    param[0] = 0.00f;  //mode
    param[1] = 0.50f;  //freq
    param[2] = 0.25f;  //freq mode
    param[3] = 0.50f;  //level (was 2)
    param[4] = 0.50f;  //level mode
    param[5] = 0.50f;  //envelope
    param[6] = 0.50f;  //gain
    strcpy(name, "Frequency/Level Splitter");
  }
private:
  friend class mdaSplitter;
  float param[NPARAMS];
  char name[32];
};


class mdaSplitter : public AudioEffectX
{
public:
  explicit mdaSplitter(audioMasterCallback audioMaster);

  void  process(const float **inputs, float **outputs, int32_t sampleFrames) override;
  void  processReplacing(const float **inputs, float **outputs, int32_t sampleFrames) override;
  void  setProgram(int32_t program) override;
  void  setProgramName(const char *name) override;
  void  getProgramName(char *name) override;
	bool getProgramNameIndexed (int32_t category, int32_t which, char* name) override;
  void  setParameter(int32_t which, float value) override;
  float getParameter(int32_t which) override;
  void  getParameterLabel(int32_t which, char *label) override;
  void  getParameterDisplay(int32_t which, char *text) override;
  void  getParameterName(int32_t which, char *text) override;
  void  suspend() override;
  void  resume() override;

	bool getEffectName(char *name) override;
	bool getVendorString(char *text) override;
	bool getProductString(char *text) override;
	int32_t getVendorVersion() override { return 1000; }

protected:
  mdaSplitterProgram programs[NPROGS];

  ///global internal variables
  float freq, fdisp, buf0, buf1, buf2, buf3;  //filter
  float level, ldisp, env, att, rel;          //level switch
  float ff, ll, pp, i2l, i2r, o2l, o2r;       //routing (freq, level, phase, output)
  int32_t  mode;

};

#endif
