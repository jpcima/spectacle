#include "SpectralAnalyzer.h"
#include <algorithm>
#include <cmath>

void BasicAnalyzer::configureBasic(uint32_t numBins)
{
    _numBins = numBins;
    _freqs.resize(numBins);
    _mags.resize(numBins);
}

void BasicAnalyzer::clear()
{
    std::fill_n(_mags.begin(), _numBins, 20.0 * std::log10(kStftFloorMagnitude));
}

///
SteppingAnalyzer::SteppingAnalyzer()
{
}

void SteppingAnalyzer::configureStepping(uint32_t numBins, const Configuration &config)
{
    configureBasic(numBins);

    const uint32_t windowSize = _windowSize = config.windowSize;
    _stepSize = config.stepSize;
    _ring.resize(2 * windowSize);
    _window.resize(windowSize);
    _input.resize(windowSize);

    float *window = _window.data();
    for (uint32_t i = 0; i < windowSize; ++i)
        window[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (windowSize - 1)));

    _smoother.configure(numBins, config.stepSize, config.attackTime, config.releaseTime, config.sampleRate);
}

void SteppingAnalyzer::setAttackAndRelease(float attack, float release)
{
    _smoother.setAttackAndRelease(attack, release);
}

void SteppingAnalyzer::clear()
{
    BasicAnalyzer::clear();

    _stepCounter = 0;
    _ringIndex = 0;
    std::fill(_ring.begin(), _ring.end(), 0.0f);

    _smoother.clear();
}

void SteppingAnalyzer::process(const float *input, uint32_t numFrames)
{
    const float* window = _window.data();
    const uint32_t windowSize = _windowSize;

    uint32_t stepCounter = _stepCounter;
    const uint32_t stepSize = _stepSize;

    float *ring = _ring.data();
    uint32_t ringIndex = _ringIndex;

    for (uint32_t i = 0; i < numFrames; ++i) {
        ring[ringIndex] = ring[ringIndex + windowSize] = input[i];
        ringIndex = (ringIndex + 1 != windowSize) ? (ringIndex + 1) : 0;
        if (++stepCounter == stepSize) {
            stepCounter = 0;

            float* windowedBlock = _input.data();
            for (uint32_t i = 0; i < windowSize; ++i)
                windowedBlock[i] = ring[ringIndex + i] * window[i];

            processNewBlock(windowedBlock);

            _smoother.process(getMagnitudes());
        }
    }

    _stepCounter = stepCounter;
    _ringIndex = ringIndex;
}

void SteppingAnalyzer::Smoother::configure(uint32_t numBins, uint32_t stepSize, double attackTime, double releaseTime, double sampleRate)
{
    _ar.resize(numBins);
    _stepSize = stepSize;
    ARFollower *ar = _ar.data();
    ar[0].init(sampleRate);
    setAttackAndRelease(attackTime, releaseTime);
}

void SteppingAnalyzer::Smoother::setAttackAndRelease(float attack, float release)
{
    ARFollower *ar = _ar.data();
    uint32_t numBins = (uint32_t)_ar.size();
    uint32_t stepSize = (uint32_t)_stepSize;
    ar[0].setAttackTime(attack / stepSize);
    ar[0].setReleaseTime(release / stepSize);
    for (uint32_t i = 1; i < numBins; ++i)
        ar[i].configureLike(ar[0]);
}

void SteppingAnalyzer::Smoother::clear()
{
    ARFollower *ar = _ar.data();
    uint32_t numBins = (uint32_t)_ar.size();
    for (uint32_t i = 0; i < numBins; ++i)
        ar[i].clear();
}

void SteppingAnalyzer::Smoother::process(float *stepData)
{
    ARFollower *ar = _ar.data();
    uint32_t numBins = (uint32_t)_ar.size();
    for (uint32_t i = 0; i < numBins; ++i)
        stepData[i] = ar[i].compute(stepData[i]);
}
