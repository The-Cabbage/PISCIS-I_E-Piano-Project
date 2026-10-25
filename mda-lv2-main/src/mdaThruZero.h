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

#ifndef MDA_THRU_ZERO_H
#define MDA_THRU_ZERO_H

#define NPARAMS  5  ///number of parameters
#define NPROGS   4  ///number of programs
#define BUFMAX   2048

#include "audioeffectx.h"

#include <cstdint>

class mdaThruZeroProgram
{
public:
  mdaThruZeroProgram();
private:
  friend class mdaThruZero;
  float param[NPARAMS];
  char name[32];
};


class mdaThruZero : public AudioEffectX
{
public:
  explicit mdaThruZero(audioMasterCallback audioMaster);
  ~mdaThruZero() override;

  void  process(const float **inputs, float **outputs, int32_t sampleFrames) override;
  void  processReplacing(const float **inputs, float **outputs, int32_t sampleFrames) override;
  void  setProgram(int32_t program) override;
  void  setProgramName(const char *name) override;
  void  getProgramName(char *name) override;
	bool getProgramNameIndexed (int32_t category, int32_t index, char* name) override;
  void  setParameter(int32_t index, float value) override;
  float getParameter(int32_t index) override;
  void  getParameterLabel(int32_t index, char *label) override;
  void  getParameterDisplay(int32_t index, char *text) override;
  void  getParameterName(int32_t index, char *text) override;
  void  suspend() override;
  void  resume() override;

	bool getEffectName(char *name) override;
	bool getVendorString(char *text) override;
	bool getProductString(char *text) override;
	int32_t getVendorVersion() override { return 1000; }

protected:
  mdaThruZeroProgram *programs;

  ///global internal variables
  float rat, dep, wet, dry, fb, dem; //rate, depth, wet & dry mix, feedback, mindepth
  float phi, fb1, fb2, deps;         //lfo & feedback buffers, depth change smoothing 
  float *buffer, *buffer2;           //maybe use 2D buffer?
	int32_t size, bufpos;
};

#endif
