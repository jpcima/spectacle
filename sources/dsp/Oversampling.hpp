#pragma once
#include "Oversampling.h"
#include "OversamplingCoefs.h"
#include <hiir/Upsampler2xFpu.h>
#include <hiir/Downsampler2xFpu.h>


///
struct UpsamplerStage2x : public hiir::Upsampler2xFpu<NC2x> {
    UpsamplerStage2x() { set_coefs(C2x); }
};

///
template <>
class Upsampler<1> final : public BasicUpsampler {
public:
    enum {
        Factor = 2,
        Log2Factor = 1,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
};

using Upsampler2x = Upsampler<1>;

///
struct UpsamplerStage4x : public hiir::Upsampler2xFpu<NC4x> {
    UpsamplerStage4x() { set_coefs(C4x); }
};

///
template <>
class Upsampler<2> final : public BasicUpsampler {
public:
    enum {
        Factor = 4,
        Log2Factor = 2,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
};

using Upsampler4x = Upsampler<2>;

///
struct UpsamplerStage8x : public hiir::Upsampler2xFpu<NC8x> {
    UpsamplerStage8x() { set_coefs(C8x); }
};

///
template <>
class Upsampler<3> final : public BasicUpsampler {
public:
    enum {
        Factor = 8,
        Log2Factor = 3,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
};

using Upsampler8x = Upsampler<3>;

///
struct UpsamplerStage16x : public hiir::Upsampler2xFpu<NC16x> {
    UpsamplerStage16x() { set_coefs(C16x); }
};

///
template <>
class Upsampler<4> final : public BasicUpsampler {
public:
    enum {
        Factor = 16,
        Log2Factor = 4,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
    UpsamplerStage16x _stage16x;
};

using Upsampler16x = Upsampler<4>;

///
struct UpsamplerStage32x : public hiir::Upsampler2xFpu<NC32x> {
    UpsamplerStage32x() { set_coefs(C32x); }
};

///
template <>
class Upsampler<5> final : public BasicUpsampler {
public:
    enum {
        Factor = 32,
        Log2Factor = 5,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
        _stage32x.process_block(outputs[4], outputs[3], count * 16);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
    UpsamplerStage16x _stage16x;
    UpsamplerStage32x _stage32x;
};

using Upsampler32x = Upsampler<5>;

///
struct UpsamplerStage64x : public hiir::Upsampler2xFpu<NC64x> {
    UpsamplerStage64x() { set_coefs(C64x); }
};

///
template <>
class Upsampler<6> final : public BasicUpsampler {
public:
    enum {
        Factor = 64,
        Log2Factor = 6,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
        _stage32x.process_block(outputs[4], outputs[3], count * 16);
        _stage64x.process_block(outputs[5], outputs[4], count * 32);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
        _stage64x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
    UpsamplerStage16x _stage16x;
    UpsamplerStage32x _stage32x;
    UpsamplerStage64x _stage64x;
};

using Upsampler64x = Upsampler<6>;

///
struct UpsamplerStage128x : public hiir::Upsampler2xFpu<NC128x> {
    UpsamplerStage128x() { set_coefs(C128x); }
};

///
template <>
class Upsampler<7> final : public BasicUpsampler {
public:
    enum {
        Factor = 128,
        Log2Factor = 7,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
        _stage32x.process_block(outputs[4], outputs[3], count * 16);
        _stage64x.process_block(outputs[5], outputs[4], count * 32);
        _stage128x.process_block(outputs[6], outputs[5], count * 64);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
        _stage64x.clear_buffers();
        _stage128x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
    UpsamplerStage16x _stage16x;
    UpsamplerStage32x _stage32x;
    UpsamplerStage64x _stage64x;
    UpsamplerStage128x _stage128x;
};

using Upsampler128x = Upsampler<7>;

///
struct DownsamplerStage2x : public hiir::Downsampler2xFpu<NC2x> {
    DownsamplerStage2x() { set_coefs(C2x); }
};

template <>
class Downsampler<1> final : public BasicDownsampler {
public:
    enum {
        Factor = 2,
        Log2Factor = 1,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
};

using Downsampler2x = Downsampler<1>;

///
struct DownsamplerStage4x : public hiir::Downsampler2xFpu<NC4x> {
    DownsamplerStage4x() { set_coefs(C4x); }
};

template <>
class Downsampler<2> final : public BasicDownsampler {
public:
    enum {
        Factor = 4,
        Log2Factor = 2,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 2);
        _stage4x.process_block(outputs[1], outputs[0], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
};

using Downsampler4x = Downsampler<2>;

///
struct DownsamplerStage8x : public hiir::Downsampler2xFpu<NC8x> {
    DownsamplerStage8x() { set_coefs(C8x); }
};

template <>
class Downsampler<3> final : public BasicDownsampler {
public:
    enum {
        Factor = 8,
        Log2Factor = 3,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 4);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
};

using Downsampler8x = Downsampler<3>;

///
struct DownsamplerStage16x : public hiir::Downsampler2xFpu<NC16x> {
    DownsamplerStage16x() { set_coefs(C16x); }
};

template <>
class Downsampler<4> final : public BasicDownsampler {
public:
    enum {
        Factor = 16,
        Log2Factor = 4,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 8);
        _stage4x.process_block(outputs[1], outputs[0], count * 4);
        _stage8x.process_block(outputs[2], outputs[1], count * 2);
        _stage16x.process_block(outputs[3], outputs[2], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;
};

using Downsampler16x = Downsampler<4>;

///
struct DownsamplerStage32x : public hiir::Downsampler2xFpu<NC32x> {
    DownsamplerStage32x() { set_coefs(C32x); }
};

template <>
class Downsampler<5> final : public BasicDownsampler {
public:
    enum {
        Factor = 32,
        Log2Factor = 5,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 16);
        _stage4x.process_block(outputs[1], outputs[0], count * 8);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 2);
        _stage32x.process_block(outputs[4], outputs[3], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;
    DownsamplerStage32x _stage32x;
};

using Downsampler32x = Downsampler<5>;

///
struct DownsamplerStage64x : public hiir::Downsampler2xFpu<NC64x> {
    DownsamplerStage64x() { set_coefs(C64x); }
};

template <>
class Downsampler<6> final : public BasicDownsampler {
public:
    enum {
        Factor = 64,
        Log2Factor = 6,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 32);
        _stage4x.process_block(outputs[1], outputs[0], count * 16);
        _stage8x.process_block(outputs[2], outputs[1], count * 8);
        _stage16x.process_block(outputs[3], outputs[2], count * 4);
        _stage32x.process_block(outputs[4], outputs[3], count * 2);
        _stage64x.process_block(outputs[5], outputs[4], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
        _stage64x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;
    DownsamplerStage32x _stage32x;
    DownsamplerStage64x _stage64x;
};

using Downsampler64x = Downsampler<6>;

///
struct DownsamplerStage128x : public hiir::Downsampler2xFpu<NC128x> {
    DownsamplerStage128x() { set_coefs(C128x); }
};

template <>
class Downsampler<7> final : public BasicDownsampler {
public:
    enum {
        Factor = 128,
        Log2Factor = 7,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 64);
        _stage4x.process_block(outputs[1], outputs[0], count * 32);
        _stage8x.process_block(outputs[2], outputs[1], count * 16);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
        _stage32x.process_block(outputs[4], outputs[3], count * 4);
        _stage64x.process_block(outputs[5], outputs[4], count * 2);
        _stage128x.process_block(outputs[6], outputs[5], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
        _stage64x.clear_buffers();
        _stage128x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;
    DownsamplerStage32x _stage32x;
    DownsamplerStage64x _stage64x;
    DownsamplerStage128x _stage128x;
};

using Downsampler128x = Downsampler<7>;
