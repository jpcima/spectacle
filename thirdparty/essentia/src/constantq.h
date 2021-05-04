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

#ifndef ESSENTIA_CONSTANTQ_H
#define ESSENTIA_CONSTANTQ_H

#include "windowing.h"
#include "fftw.h"
#include "fftwcomplex.h"
#include "essentiamath.h"
#include <string>
#include <complex>
#include <vector>


namespace essentia {
namespace standard {

class ConstantQ {
 protected:
  using Real = float;

  FFTWComplex* _fftc;
  Windowing* _windowing;
  FFTW* _fft;

  std::vector<double> _CQdata;
  std::vector<std::complex<Real> > _fftData;

  double _sampleRate;
  double _minFrequency;
  double _maxFrequency;
  double _Q;            // constant Q factor
  double _threshold;    // threshold for kernel generation
  double _scale;

  unsigned int _numWin;
  unsigned int _binsPerOctave;
  unsigned int _windowSize;
  unsigned int _inputFFTSize;
  unsigned int _numberBins;
  unsigned int _minimumKernelSize;

  bool _zeroPhase;

  struct SparseKernel {
    std::vector<double> real;
    std::vector<double> imag;
    std::vector<unsigned> i;
    std::vector<unsigned> j;
  };

  SparseKernel _sparseKernel;


 public:
  ConstantQ() {
    _fftc = new FFTWComplex; //FFT with complex input
    _windowing = new Windowing;
    _fft = new FFTW;
  }

  ~ConstantQ() {
    delete _fftc;
    delete _windowing;
    delete _fft;
  }

  struct Config {
    double sampleRate = 44100;
    // minimum frequency [Hz] [1,inf)
    double minFrequency = 32.7;
    // number of frequency bins, starting at minFrequency [1,inf)
    unsigned int numberBins = 84;
    // number of bins per octave [1,inf)
    unsigned int binsPerOctave = 12;
    // bins whose magnitude is below this quantile are discarded [0,1)
    double threshold = 0.01;
    // filters scale. Larger values use longer windows [0,inf)
    double scale = 1.0;
    // the window type {hamming,hann,hannnsgcq,triangular,square,blackmanharris62,blackmanharris70,blackmanharris74,blackmanharris92}
    std::string windowType = "hann";
    // minimum size allowed for frequency kernels [2,inf)
    unsigned int minimumKernelSize = 4;
    // a boolean value that enables zero-phase windowing. Input audio frames should be windowed with the same phase mode {true,false}
    bool zeroPhase = true;
  };

  void compute(
    // the windowed input audio frame
    const std::vector<Real>& frame,
    // the Constant Q transform
    std::vector<std::complex<Real>>& constantQ);
  void configure(const Config& config);
};

} // namespace standard
} // namespace essentia


#endif // ESSENTIA_CONSTANTQ_H
