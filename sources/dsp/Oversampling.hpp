#pragma once
#include "Oversampling.h"

///
template <>
class Upsampler<1> : public BasicUpsampler {
private:
    UpsamplerStage2x _stage;

public:
    virtual ~Upsampler() = default;

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage.process_block(outputs[0], input, count);
    }

    void clear() override
    {
        _stage.clear_buffers();
    }
};

///
template <>
class Upsampler<2> : public Upsampler<1> {
private:
    UpsamplerStage4x _stage;

public:
    virtual ~Upsampler() = default;

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        Upsampler<1>::upsample(count, input, outputs);
        _stage.process_block(outputs[1], outputs[0], 2 * count);
    }

    void clear() override
    {
        Upsampler<1>::clear();
        _stage.clear_buffers();
    }
};

///
template <>
class Upsampler<3> : public Upsampler<2> {
private:
    UpsamplerStage8x _stage;

public:
    virtual ~Upsampler() = default;

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        Upsampler<2>::upsample(count, input, outputs);
        _stage.process_block(outputs[2], outputs[1], 4 * count);
    }

    void clear() override
    {
        Upsampler<2>::clear();
        _stage.clear_buffers();
    }
};

///
template <>
class Upsampler<4> : public Upsampler<3> {
private:
    UpsamplerStage16x _stage;

public:
    virtual ~Upsampler() = default;

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        Upsampler<3>::upsample(count, input, outputs);
        _stage.process_block(outputs[3], outputs[2], 8 * count);
    }

    void clear() override
    {
        Upsampler<3>::clear();
        _stage.clear_buffers();
    }
};

///
template <>
class Downsampler<1> : public BasicDownsampler {
protected:
    DownsamplerStage2x _stage2x;

public:
    virtual ~Downsampler() = default;

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
    }
};

///
template <>
class Downsampler<2> : public BasicDownsampler {
protected:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;

public:
    virtual ~Downsampler() = default;

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
};

///
template <>
class Downsampler<3> : public BasicDownsampler {
protected:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;

public:
    virtual ~Downsampler() = default;

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
};

///
template <>
class Downsampler<4> : public BasicDownsampler {
protected:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;

public:
    virtual ~Downsampler() = default;

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
};
