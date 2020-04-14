#pragma once
#include <cstdint>

static constexpr uint32_t kStftMinSizeLog2 = 8;
static constexpr uint32_t kStftMaxSizeLog2 = 14;
static constexpr uint32_t kStftDefaultSizeLog2 = 10;

static constexpr uint32_t kStftNumSizes = 1 + kStftMaxSizeLog2 - kStftMinSizeLog2;

static constexpr uint32_t kStftMinSize = 1u << kStftMinSizeLog2;
static constexpr uint32_t kStftMaxSize = 1u << kStftMaxSizeLog2;
static constexpr uint32_t kStftDefaultSize = 1u << kStftDefaultSizeLog2;

static constexpr uint32_t kStftStepSize = 64;

static constexpr double kStftMinAttackTime = 1e-3;
static constexpr double kStftMaxAttackTime = 100e-3;
static constexpr double kStftMinReleaseTime = 1e-3;
static constexpr double kStftMaxReleaseTime = 500e-3;
static constexpr double kStftDefaultAttackTime = 20e-3;
static constexpr double kStftDefaultReleaseTime = 250e-3;

static constexpr double kStftFloorMagnitude = 1e-9;
static constexpr double kStftFloorMagnitudeInDB = -180.0;

static constexpr double kNegligibleDB = 0.01;
