#pragma once
#include "DistrhoPlugin.hpp"

enum {
    kPidFftSize,
    kPidStepSize,
    kPidAttackTime,
    kPidReleaseTime,
    kParameterCount,
};

void InitParameter(uint32_t index, Parameter &parameter);
