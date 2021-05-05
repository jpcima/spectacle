#include "STFT.h"
#include "FFTPlanner.h"
#include "AnalyzerDefs.h"
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <cmath>

void STFT::configure(const Configuration &config)
{
    const uint32_t windowSize = config.windowSize;
    const uint32_t numBins = windowSize / 2 + 1;
    configureStepping(numBins, config);

    const double sampleRate = config.sampleRate;
    _sampleRate = sampleRate;
    _fftPlan = FFTPlanner::getInstance().forwardFFT(windowSize);
    _cpx.resize(numBins);

    float *frequencies = getFrequencies();
    for (uint32_t i = 0; i < numBins; ++i)
        frequencies[i] = (float)(i * sampleRate / windowSize);
}

void STFT::processNewBlock(float *input)
{
    const uint32_t windowSize = getWindowSize();
    const uint32_t numBins = windowSize / 2 + 1;

    fftwf_plan plan = _fftPlan;
    std::complex<float> *cpx = _cpx.data();
    fftwf_execute_dft_r2c(plan, input, (fftwf_complex *)cpx);

    float *mag = getMagnitudes();
    for (uint32_t i = 0; i < numBins; ++i) {
        double linear = std::abs(cpx[i]) * (2.0f / windowSize);
        double decibel = 20.0 * std::log10(std::max(kStftFloorMagnitude, linear));
        mag[i] = decibel;
    }
}
