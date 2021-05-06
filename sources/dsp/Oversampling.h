#pragma once
#include "OversamplingStages.h"
#include <hiir/Upsampler2xFpu.h>
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
using Upsampler2x = Upsampler<1>;
using Upsampler4x = Upsampler<2>;
using Upsampler8x = Upsampler<3>;
using Upsampler16x = Upsampler<4>;

///
class BasicDownsampler {
public:
    virtual ~BasicDownsampler() {}
    virtual void downsample(uint32_t count, const float *input, float **outputs) = 0;
    virtual void clear() = 0;
};

///
template <uint32_t Log2Factor> class Downsampler;
using Downsampler2x = Downsampler<1>;
using Downsampler4x = Downsampler<2>;
using Downsampler8x = Downsampler<3>;
using Downsampler16x = Downsampler<4>;

///
#include "Oversampling.hpp"
