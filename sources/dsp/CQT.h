#pragma once
#include "ARFollower.h"
#include <constantq.h>
#include <vector>
#include <memory>
#include <complex>
#include <cstdint>

class CQT {
public:
    CQT();
    ~CQT();

    void configure(uint32_t fftSize, uint32_t stepSize, double attackTime, double releaseTime, double sampleRate);
    void process(const float *input, uint32_t numFrames);

    const float* getFrequencies() { return fFrequencies.data(); }
    const float* getMagnitudes() { return fMagnitudes.data(); }
    uint32_t getNumBins() const { return (uint32_t)fFrequencies.size(); }

private:
    void processNewWindow(const float *rawInput);

private:
    using ConstantQ = essentia::standard::ConstantQ;

    ConstantQ fCq;
    std::vector<float> fInput;
    std::vector<std::complex<float>> fOutput;
    std::vector<float> fFrequencies;
    std::vector<float> fMagnitudes;
    ConstantQ::Config fConfig;
    uint32_t fFftSize = 0;
    uint32_t fStepSize = 0;
    uint32_t fStepCounter {};
    uint32_t fRingIndex {};
    std::vector<float> fRing;
    std::vector<float> fWindow;
    std::vector<ARFollower> fARSmooth;
};
