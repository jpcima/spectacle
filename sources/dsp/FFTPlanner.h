#pragma once
#include "FFT_util.h"
#include <map>
#include <mutex>

class FFTPlanner {
private:
    FFTPlanner() = default;

public:
    static FFTPlanner& getInstance();
    fftwf_plan forwardFFT(uint32_t windowSize);

private:
    std::mutex _mutex;
    std::map<uint32_t, fftwf_plan_u> _forwardPlans;
};
