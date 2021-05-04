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

#ifndef ESSENTIA_FFTWCOMPLEX_H
#define ESSENTIA_FFTWCOMPLEX_H

#include <complex>
#include <vector>
#include <fftw3.h>

namespace essentia {
namespace standard {

class FFTWComplex {

 protected:
  using Real = float;

 public:
  FFTWComplex() : _fftPlan(0), _input(0), _output(0) {
  }

  ~FFTWComplex();

  struct Config {
    // the expected size of the input frame. This is purely optional and only targeted at optimizing the creation time of the FFT object [1,inf)
    int size = 1024;
    // returns the full spectrum or just the positive frequencies {true,false}
    bool negativeFrequencies = false;
  };

  void compute(
    // the input frame (complex)
    const std::vector<std::complex<Real> >&  signal,
    // the FFT of the input frame
    std::vector<std::complex<Real> >& _fft);
  void configure(const Config& config);

  static const char* name;
  static const char* category;
  static const char* description;

 protected:
  fftwf_plan _fftPlan;
  int _fftPlanSize;
  std::complex<Real>* _input;
  std::complex<Real>* _output;

  bool _negativeFrequencies;

  void createFFTObject(int size);
};

} // namespace standard
} // namespace essentia

#endif // ESSENTIA_FFTWCOMPLEX_H
