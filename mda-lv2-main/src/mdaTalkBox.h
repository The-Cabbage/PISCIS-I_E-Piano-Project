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

#include "audioeffectx.h"

#include <cstdint>

#define NPARAMS            4  ///number of parameters
#define NPROGS             1  ///number of programs
#define BUF_MAX         1600
#define ORD_MAX           50
#define TWO_PI     6.28318530717958647692528676655901f


class mdaTalkBoxProgram
{
public:
  mdaTalkBoxProgram();

private:
  friend class mdaTalkBox;
  float param[NPARAMS];
  char name[24];
};


class mdaTalkBox : public AudioEffectX
{
public:
  explicit mdaTalkBox(audioMasterCallback audioMaster);
  ~mdaTalkBox() override;

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
  mdaTalkBoxProgram *programs;

  void lpc(float *buf, const float *car, int32_t n, int32_t o);
  void lpc_durbin(const float *r, int p, float *k, float *g);

  ///global internal variables
  float *car0, *car1;
  float *window;
  float *buf0, *buf1;
  
  float emphasis;
  int32_t K, N, O, pos, swap;
  float wet, dry, FX;

  float d0, d1, d2, d3, d4;
  float u0, u1, u2, u3, u4;
};


