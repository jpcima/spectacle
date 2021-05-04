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

#ifndef ESSENTIA_FFTW_H
#define ESSENTIA_FFTW_H

#include <mutex>
#include <complex>
#include <vector>
#include <fftw3.h>

namespace essentia {
namespace standard {

class FFTW {

 protected:
  using Real = float;

 public:
  FFTW() : _fftPlan(0), _input(0), _output(0) {
  }

  ~FFTW();

  struct Config {
    // the expected size of the input frame. This is purely optional and only targeted at optimizing the creation time of the FFT object [1,inf)
    int size = 1024;
  };

  void compute(
    // the input audio frame
    const std::vector<Real>& signal,
    // the FFT of the input frame
    std::vector<std::complex<Real> >& fft);
  void configure(const Config& config);

 protected:
  friend class IFFTW;
  friend class FFTWComplex;
  friend class IFFTWComplex;
  static std::mutex globalFFTWMutex;

  fftwf_plan _fftPlan;
  int _fftPlanSize;
  Real* _input;
  std::complex<Real>* _output;

  void createFFTObject(int size);
};

} // namespace standard
} // namespace essentia

#endif // ESSENTIA_FFTW_H
