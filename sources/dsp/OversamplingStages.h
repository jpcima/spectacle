#pragma once
#include "OversamplingCoefs.h"
#include <hiir/Upsampler2xFpu.h>
#include <hiir/Downsampler2xFpu.h>

struct UpsamplerStage2x : public hiir::Upsampler2xFpu<NC2x> {
    UpsamplerStage2x() { set_coefs(C2x); }
};
struct UpsamplerStage4x : public hiir::Upsampler2xFpu<NC4x> {
    UpsamplerStage4x() { set_coefs(C4x); }
};
struct UpsamplerStage8x : public hiir::Upsampler2xFpu<NC8x> {
    UpsamplerStage8x() { set_coefs(C8x); }
};
struct UpsamplerStage16x : public hiir::Upsampler2xFpu<NC16x> {
    UpsamplerStage16x() { set_coefs(C16x); }
};

struct DownsamplerStage2x : public hiir::Downsampler2xFpu<NC2x> {
    DownsamplerStage2x() { set_coefs(C2x); }
};
struct DownsamplerStage4x : public hiir::Downsampler2xFpu<NC4x> {
    DownsamplerStage4x() { set_coefs(C4x); }
};
struct DownsamplerStage8x : public hiir::Downsampler2xFpu<NC8x> {
    DownsamplerStage8x() { set_coefs(C8x); }
};
struct DownsamplerStage16x : public hiir::Downsampler2xFpu<NC16x> {
    DownsamplerStage16x() { set_coefs(C16x); }
};
