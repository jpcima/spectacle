#pragma once
#include "FFT_util.h"
#include <vector>
#include <cstdint>

class STFT {
public:
    STFT();
    ~STFT();

    void configure(uint32_t fftSize, uint32_t stepSize, double smoothTime, double sampleRate);
    void clear();
    void process(const float *input, uint32_t numFrames);

    const float *getFrequencies() const { return _outputFrequencies.data(); }
    const float *getMagnitudes() const { return _outputMagnitudes.data(); }
    uint32_t getFftSize() const { return _fftSize; }

private:
    void processNewWindow(const float *rawInput);

private:
    static fftwf_plan prepareFFT(uint32_t fftSize);

private:
    static constexpr bool kOutputAsDecibels = true;
    static constexpr bool kOutputNoDcComponent = false;

private:
    uint32_t _fftSize {};
    fftwf_plan _fftPlan {};
    double _sampleRate {};

    // analysis step
    uint32_t _stepCounter {};
    uint32_t _stepSize {};

    // input sample accumulation
    uint32_t _ringIndex {};
    std::vector<float> _ring;

    fftwf_real_vector _real;
    fftwf_complex_vector _cpx;

    // window
    std::vector<float> _window;

    // output buffers
    std::vector<float> _outputFrequencies;
    std::vector<float> _outputMagnitudes;

    // smooth
    float _smoothPole {};
};
