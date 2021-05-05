#pragma once
#include "DistrhoPlugin.hpp"

enum {
    kPidFftSize,
    kPidStepSize,
    kPidAttackTime,
    kPidReleaseTime,
    kPidAlgorithm,
    kParameterCount,
};

void InitParameter(uint32_t index, Parameter &parameter);
