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

#ifndef MDA_VOCODER_H
#define MDA_VOCODER_H

#define NPARAMS  8  ///number of parameters
#define NPROGS   5  ///number of programs
#define NBANDS  16  ///max vocoder bands

#include "audioeffectx.h"

#include <cstdint>

class mdaVocoderProgram
{
public:
  mdaVocoderProgram();
private:
  friend class mdaVocoder;
  float param[NPARAMS];
  char name[32];
};


class mdaVocoder : public AudioEffectX
{
public:
  explicit mdaVocoder(audioMasterCallback audioMaster);
  ~mdaVocoder() override;

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
  mdaVocoderProgram *programs;

  ///global internal variables
  int32_t  swap;       //input channel swap
  float gain;       //output level
  float thru, high; //hf thru              
  float kout; //downsampled output
  int32_t  kval; //downsample counter
  int32_t  nbnd; //number of bands
  
  //filter coeffs and buffers - seems it's faster to leave this global than make local copy 
  float f[NBANDS][13]; //[0-8][0 1 2 | 0 1 2 3 | 0 1 2 3 | val rate]
};                     //  #   reson | carrier |modulator| envelope

#endif
