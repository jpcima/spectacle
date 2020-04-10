#pragma once
#include <cstdint>

static constexpr uint32_t kStftMinSizeLog2 = 8;
static constexpr uint32_t kStftMaxSizeLog2 = 16;
static constexpr uint32_t kStftDefaultSizeLog2 = 10;

static constexpr uint32_t kStftNumSizes = 1 + kStftMaxSizeLog2 - kStftMinSizeLog2;

static constexpr uint32_t kStftMinSize = 1u << kStftMinSizeLog2;
static constexpr uint32_t kStftMaxSize = 1u << kStftMaxSizeLog2;
static constexpr uint32_t kStftDefaultSize = 1u << kStftDefaultSizeLog2;

static constexpr uint32_t kStftStepSize = 64;

static constexpr double kStftSmoothTime = 250e-3;

static constexpr double kStftFloorMagnitude = 1e-9;
