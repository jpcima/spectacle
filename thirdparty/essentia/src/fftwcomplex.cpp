/*
 * Copyright (C) 2006-2021  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include "fftwcomplex.h"
#include "fftw.h"
#include <stdexcept>
#include <cstring>

using namespace std;
using namespace essentia;
using namespace standard;


FFTWComplex::~FFTWComplex() {
  std::lock_guard<std::mutex> lock(FFTW::globalFFTWMutex);

  fftwf_destroy_plan(_fftPlan);
  fftwf_free(_input);
  fftwf_free(_output);
}

void FFTWComplex::compute(
  const std::vector<std::complex<Real> >& signal,
  std::vector<std::complex<Real> >& fft) {

  // check if input is OK
  int size = int(signal.size());
  if (size == 0) {
    throw std::runtime_error("FFT: Input size cannot be 0");
  }

  if ((_fftPlan == 0) ||
      ((_fftPlan != 0) && _fftPlanSize != size)) {
    createFFTObject(size);
  }

  // copy input into plan
  memcpy(_input, &signal[0], size*sizeof(complex<Real>));

  // calculate the fft
  fftwf_execute(_fftPlan);

  // copy result from plan to output vector
  if (_negativeFrequencies){
    fft.resize(size);
    memcpy(&fft[0], _output, (size)*sizeof(complex<Real>));
  }
  else{
    fft.resize(size/2+1);
    memcpy(&fft[0], _output, (size/2+1)*sizeof(complex<Real>));
  }
}

void FFTWComplex::configure(const Config& config) {
  createFFTObject(config.size);
  _negativeFrequencies = config.negativeFrequencies;
}

void FFTWComplex::createFFTObject(int size) {
  std::lock_guard<std::mutex> lock(FFTW::globalFFTWMutex);

  // This is only needed because at the moment we return half of the spectrum,
  // which means that there are 2 different input signals that could yield the
  // same FFT...
  if (size % 2 == 1) {
    throw std::runtime_error("FFT: can only compute FFT of arrays which have an even size");
  }

  // create the temporary storage array
  fftwf_free(_input);
  fftwf_free(_output);
  _input = (complex<Real>*)fftwf_malloc(sizeof(complex<Real>)*size);
  _output = (complex<Real>*)fftwf_malloc(sizeof(complex<Real>)*size);

  if (_fftPlan != 0) {
    fftwf_destroy_plan(_fftPlan);
  }

    int planFlags = 0;
#if defined(USE_IMPATIENT_FFT_PLANNING)
    planFlags |= FFTW_ESTIMATE;
#else
    planFlags |= FFTW_MEASURE;
#endif
  _fftPlan = fftwf_plan_dft_1d(size, (fftwf_complex*)_input, (fftwf_complex*)_output, FFTW_FORWARD, planFlags);
  _fftPlanSize = size;
}
