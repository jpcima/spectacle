#include "CQT.h"
#include "AnalyzerDefs.h"
#include <algorithm>
#include <cmath>

CQT::CQT()
{
    fInput.reserve(1024);
}

CQT::~CQT()
{
}

void CQT::configure(uint32_t fftSize, uint32_t stepSize, double attackTime, double releaseTime, double sampleRate)
{
    fConfig.sampleRate = sampleRate;

    // WS = Q * Fs / Fmin
    // Q = Scale / 2^((1/BinsPerOctave)-1)

    const double Q = fftSize * fConfig.minFrequency / sampleRate;
    fConfig.scale = Q * (std::pow(2.0, (1.0 / fConfig.binsPerOctave)) - 1.0);

    //
    const uint32_t numBins = fConfig.numberBins;
    fInput.resize(fftSize);
    fOutput.resize(numBins);
    fFrequencies.resize(numBins);
    fMagnitudes.resize(numBins);

    fCq.configure(fConfig);

    //
    fFftSize = fftSize;
    fStepSize = stepSize;

    fStepCounter = 0;
    fRingIndex = 0;
    fRing.resize(2 * fftSize);

    fARSmooth.resize(numBins);

    ARFollower *ar = fARSmooth.data();
    ar[0].init(sampleRate);
    ar[0].setAttackTime(attackTime / stepSize);
    ar[0].setReleaseTime(releaseTime / stepSize);
    for (uint32_t i = 1; i < numBins; ++i)
        ar[i].configureLike(ar[0]);

    //
    if (fWindow.size() != fftSize) {
        fWindow.resize(fftSize);
        float *window = fWindow.data();
        for (uint32_t i = 0; i < fftSize; ++i)
            window[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (fftSize - 1)));
    }
}

void CQT::process(const float *input, uint32_t numFrames)
{
    const uint32_t fftSize = fFftSize;

    uint32_t stepCounter = fStepCounter;
    const uint32_t stepSize = fStepSize;

    float *ring = fRing.data();
    uint32_t ringIndex = fRingIndex;

    for (uint32_t i = 0; i < numFrames; ++i) {
        ring[ringIndex] = ring[ringIndex + fftSize] = input[i];
        ringIndex = (ringIndex + 1 != fftSize) ? (ringIndex + 1) : 0;
        if (++stepCounter == stepSize) {
            stepCounter = 0;
            processNewWindow(&ring[ringIndex]);
        }
    }

    fStepCounter = stepCounter;
    fRingIndex = ringIndex;
}

void CQT::processNewWindow(const float *rawInput)
{
    const uint32_t fftSize = fFftSize;
    float *input = fInput.data();
    std::complex<float>* output = fOutput.data();
    const float* window = fWindow.data();

    for (uint32_t i = 0; i < fftSize; ++i)
        input[i] = window[i] * rawInput[i];

    fCq.compute(fInput, fOutput);

    ///
    const float minFrequency = fConfig.minFrequency;
    const uint32_t numBins = fConfig.numberBins;
    const uint32_t binsPerOctave = fConfig.binsPerOctave;

    float *frequencies = fFrequencies.data();
    float *magnitudes = fMagnitudes.data();
    ARFollower *ar = fARSmooth.data();

    for (uint32_t i = 0; i < numBins; ++i) {
        double curMag = std::abs(output[i]);
        curMag = 20.0 * std::log10(std::max(kStftFloorMagnitude, curMag));
        frequencies[i] = minFrequency * std::pow(2.0f, (i * (1.0f / binsPerOctave)));
        magnitudes[i] = ar[i].compute(curMag);
    }
}
