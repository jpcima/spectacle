#pragma once
#include "DistrhoPlugin.hpp"

enum {
    kPidFftSize,
    kPidReleaseTime,
    kParameterCount,
};

void InitParameter(uint32_t index, Parameter &parameter);
