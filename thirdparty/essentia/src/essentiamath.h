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

#ifndef ESSENTIA_MATH_H
#define ESSENTIA_MATH_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <cmath>
#include <vector>

namespace essentia {

/**
 * Return the next power of two after the given number n.
 * If n is already a power of two, return n.
 */
template <typename T> T nextPowerTwo(T n) {
  n--;
  n |= (n >> 1);
  n |= (n >> 2);
  n |= (n >> 4);
  n |= (n >> 8);
  n |= (n >> 16);
  return ++n;
}

template <> inline long long int nextPowerTwo(long long int n) {
  n--;
  n |= (n >> 1);
  n |= (n >> 2);
  n |= (n >> 4);
  n |= (n >> 8);
  n |= (n >> 16);
  n |= (n >> 32);
  return ++n;
}

/**
 * Returns the sum of squared values of an array
 */
template <typename T> T sumSquare(const std::vector<T> array) {
  T sum = 0.0;
  for (size_t i = 0; i < array.size(); ++i) {
    sum += array[i] * array[i];
  }
  return sum;
}

/**
 * returns the sum of an array, unrolled version.
 */
template <typename T> T sum(const std::vector<T>& array, int start, int end) {
  T sum = 0.0;
  int i = start;

  for (; i<end-8; i+=8) {
    sum += array[i];
    sum += array[i+1];
    sum += array[i+2];
    sum += array[i+3];
    sum += array[i+4];
    sum += array[i+5];
    sum += array[i+6];
    sum += array[i+7];
  }

  // do the rest of the loop
  for (; i<end; i++) {
    sum += array[i];
  }

  return sum;
}

/**
 * returns the sum of an array.
 */
template <typename T> T sum(const std::vector<T>& array) {
  if (array.empty()) return 0;
  return sum(array, 0, array.size());
}

// normalize a vector so it's sum is equal to 1. the vector is not touched if
// it contains negative elements or the sum is zero
template <typename T> void normalizeSum(std::vector<T>& array) {
  if (array.empty()) return;

  //T sumElements = std::accumulate(array.begin(), array.end(), (T) 0.0);
  T sumElements = (T) 0.;
  for (size_t i=0; i<array.size(); ++i) {
    if (array[i] < 0) return;
    sumElements += array[i];
  }

  if (sumElements != (T) 0.0) {
    for (size_t i=0; i<array.size(); ++i) {
      array[i] /= sumElements;
    }
  }
}

} // namespace essentia

#endif // ESSENTIA_MATH_H
