#pragma once
#include <cstdint>

///
class BasicUpsampler {
public:
    virtual ~BasicUpsampler() {}
    virtual void upsample(uint32_t count, const float *input, float **outputs) = 0;
    virtual void clear() = 0;
};

///
template <uint32_t Log2Factor> class Upsampler;

///
class BasicDownsampler {
public:
    virtual ~BasicDownsampler() {}
    virtual void downsample(uint32_t count, const float *input, float **outputs) = 0;
    virtual void clear() = 0;
};

///
template <uint32_t Log2Factor> class Downsampler;

///
#include "Oversampling.hpp"
