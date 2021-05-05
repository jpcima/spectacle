#pragma once
#include "SpectralAnalyzer.h"
#include "FFT_util.h"
#include <vector>
#include <cstdint>

class STFT final : public SteppingAnalyzer {
public:
    void configure(const Configuration &config) override;

private:
    void processNewBlock(float *input) override;

private:
    fftwf_plan _fftPlan {};
    double _sampleRate {};

    // temporary
    fftwf_complex_vector _cpx;
};
