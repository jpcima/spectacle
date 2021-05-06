#pragma once
#include <cstdint>

enum Algorithm {
    kAlgoStft,
    kAlgoMultirateStftX2,
    kAlgoMultirateStftX3,
    kAlgoMultirateStftX4,
    kAlgoMultirateStftX5,
    kAlgoMultirateStftX6,
    kAlgoMultirateStftX7,
    kAlgoMultirateStftX8,
    kNumAlgorithms,
};

static constexpr Algorithm kDefaultAlgorithm = kAlgoMultirateStftX6;

inline const char *getAlgorithmName(Algorithm a)
{
    switch (a) {
    case kAlgoStft: default:
        return "STFT";
    case kAlgoMultirateStftX2:
        return "STFT x2";
    case kAlgoMultirateStftX3:
        return "STFT x3";
    case kAlgoMultirateStftX4:
        return "STFT x4";
    case kAlgoMultirateStftX5:
        return "STFT x5";
    case kAlgoMultirateStftX6:
        return "STFT x6";
    case kAlgoMultirateStftX7:
        return "STFT x7";
    case kAlgoMultirateStftX8:
        return "STFT x8";
    }
}

static constexpr uint32_t kStftMinSizeLog2 = 6;
static constexpr uint32_t kStftMaxSizeLog2 = 14;
static constexpr uint32_t kStftDefaultSizeLog2 = 8;

static constexpr uint32_t kStftNumSizes = 1 + kStftMaxSizeLog2 - kStftMinSizeLog2;

static constexpr uint32_t kStftMinSize = 1u << kStftMinSizeLog2;
static constexpr uint32_t kStftMaxSize = 1u << kStftMaxSizeLog2;
static constexpr uint32_t kStftDefaultSize = 1u << kStftDefaultSizeLog2;

static constexpr uint32_t kStftMinStepLog2 = 6;
static constexpr uint32_t kStftMaxStepLog2 = 12;
static constexpr uint32_t kStftDefaultStepLog2 = 8;

static constexpr uint32_t kStftNumSteps = 1 + kStftMaxStepLog2 - kStftMinStepLog2;

static constexpr uint32_t kStftMinStep = 1u << kStftMinStepLog2;
static constexpr uint32_t kStftMaxStep = 1u << kStftMaxStepLog2;
static constexpr uint32_t kStftDefaultStep = 1u << kStftDefaultStepLog2;

static constexpr double kStftMinAttackTime = 1e-3;
static constexpr double kStftMaxAttackTime = 100e-3;
static constexpr double kStftMinReleaseTime = 1e-3;
static constexpr double kStftMaxReleaseTime = 500e-3;
static constexpr double kStftDefaultAttackTime = 20e-3;
static constexpr double kStftDefaultReleaseTime = 250e-3;

static constexpr double kStftFloorMagnitude = 1e-9;
static constexpr double kStftFloorMagnitudeInDB = -180.0;

static constexpr double kNegligibleDB = 0.01;
