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

#ifndef MDA_VOC_INPUT_H
#define MDA_VOC_INPUT_H

#define NPARAMS  5  ///number of parameters
#define NPROGS   1  ///number of programs

#include "audioeffectx.h"

#include <cstdint>
#include <random>

class mdaVocInputProgram
{
public:
  mdaVocInputProgram();
private:
  friend class mdaVocInput;
  float param[NPARAMS];
  char name[32];
};


class mdaVocInput : public AudioEffectX
{
public:
  explicit mdaVocInput(audioMasterCallback audioMaster);
  ~mdaVocInput() override;

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
  virtual void  midi2string(int32_t n, char *text);
  void  suspend() override;
  void  resume() override;

	bool getEffectName(char *name) override;
	bool getVendorString(char *text) override;
	bool getProductString(char *text) override;
	int32_t getVendorVersion() override { return 1000; }

protected:
  std::minstd_rand rng;
  std::uniform_real_distribution<float> dist;

  mdaVocInputProgram *programs;

  ///global internal variables
  int32_t  track;        //track input pitch
  float pstep;        //output sawtooth inc per sample
  float pmult;        //tuning multiplier
  float sawbuf;   
  float noise;        //breath noise level
  float lenv, henv;   //LF and overall envelope
  float lbuf0, lbuf1; //LF filter buffers
  float lbuf2;        //previous LF sample
  float lbuf3;        //period measurement
  float lfreq;        //LF filter coeff
  float vuv;          //voiced/unvoiced threshold
  float maxp, minp;   //preferred period range
  double root;        //tuning reference (MIDI note 0 in Hz)
};

#endif
