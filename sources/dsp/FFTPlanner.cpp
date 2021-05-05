#include "FFTPlanner.h"

FFTPlanner& FFTPlanner::getInstance()
{
    static FFTPlanner instance;
    return instance;
}

fftwf_plan FFTPlanner::forwardFFT(uint32_t windowSize)
{
    std::unique_lock<std::mutex> lock(_mutex);

    auto it = _forwardPlans.find(windowSize);
    if (it != _forwardPlans.end())
        return it->second.get();

    const uint32_t numBins = windowSize / 2 + 1;
    fftwf_real_vector real(windowSize);
    fftwf_complex_vector cpx(numBins);

    int planFlags = 0;
#if defined(USE_IMPATIENT_FFT_PLANNING)
    planFlags |= FFTW_ESTIMATE;
#else
    planFlags |= FFTW_MEASURE;
#endif
    fftwf_plan plan = fftwf_plan_dft_r2c_1d(windowSize, real.data(), (fftwf_complex *)cpx.data(), planFlags);
    _forwardPlans[windowSize] = fftwf_plan_u(plan);
    return plan;
}
