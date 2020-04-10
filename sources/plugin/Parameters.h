#pragma once
#include "DistrhoPlugin.hpp"

enum {
    kPidFftSize,
    kParameterCount,
};

void InitParameter(uint32_t index, Parameter &parameter);
