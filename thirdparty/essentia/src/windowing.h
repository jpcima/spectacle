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

#ifndef ESSENTIA_WINDOWING_H
#define ESSENTIA_WINDOWING_H

#include <string>
#include <vector>

namespace essentia {
namespace standard {

class Windowing {

 protected:
  using Real = float;

 public:
  struct Config {
    // the window size [2,inf)
    int size = 1024;
    // the size of the zero-padding [0,inf)
    int zeroPadding = 0;
    // the window type {hamming,hann,hannnsgcq,triangular,square,blackmanharris62,blackmanharris70,blackmanharris74,blackmanharris92}
    std::string type = "hann";
    // a boolean value that enables zero-phase windowing {true,false}
    bool zeroPhase = true;
    // a boolean value to specify whether to normalize windows (to have an area of 1) and then scale by a factor of 2 {true,false}
    bool normalized = true;
  };

  void configure(const Config& config);

  void compute(
    // the input audio frame
    const std::vector<Real>& frame,
    // the windowed audio frame
    std::vector<Real>& windowedFrame);

protected:
  void createWindow(const std::string& windowtype);

  // window generators
  void hamming();
  void hann();
  void hannNSGCQ();
  void triangular();
  void square();
  void normalize();
  void blackmanHarris(double a0, double a1, double a2, double a3 = 0.0);
  void blackmanHarris62();
  void blackmanHarris70();
  void blackmanHarris74();
  void blackmanHarris92();

  void makeZeroPhase();

  std::vector<Real> _window;
  int _zeroPadding;
  std::string _type;
  bool _zeroPhase;
  bool _normalized;
};

} // namespace standard
} // namespace essentia

#endif // ESSENTIA_WINDOWING_H
