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

#ifndef MDA_LESLIE_H
#define MDA_LESLIE_H

#include "audioeffectx.h"

#include <cstdint>

#define NPARAMS  9      //number of parameters
#define NPROGS   3      //number of programs

class mdaLeslieProgram
{
public:
	mdaLeslieProgram();
private:
	friend class mdaLeslie;
    float param[NPARAMS];
	char name[24];
};

class mdaLeslie : public AudioEffectX
{
public:
	explicit mdaLeslie(audioMasterCallback audioMaster);
	~mdaLeslie() override;

	void process(const float **inputs, float **outputs, int32_t sampleFrames) override;
	void processReplacing(const float **inputs, float **outputs, int32_t sampleFrames) override;
	void setProgram(int32_t program) override;
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
    void update();
	mdaLeslieProgram *programs;

  float filo; //crossover filter coeff
  float fbuf1, fbuf2; //filter buffers
  float twopi; //speed, target, momentum, phase, width, ampmod, freqmod...
  float hspd, hset, hmom, hphi, hwid, hlev, hdep;
  float lspd, lset, lmom, lphi, lwid, llev, gain;
  float *hbuf;  //HF delay buffer
	int32_t size, hpos; //buffer length & pointer

  float chp, dchp, clp, dclp, shp, dshp, slp, dslp;
};

#endif
