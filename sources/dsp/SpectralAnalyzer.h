#pragma once
#include "AnalyzerDefs.h"
#include "ARFollower.h"
#include <vector>
#include <cstdint>

///
struct Configuration {
    uint32_t windowSize = kStftDefaultSize;
    uint32_t stepSize = kStftDefaultStep;
    double attackTime = kStftDefaultAttackTime;
    double releaseTime = kStftDefaultReleaseTime;
    double sampleRate = 44100.0;
};

///
class BasicAnalyzer {
public:
    virtual ~BasicAnalyzer() {}

protected:
    void configureBasic(uint32_t numBins);

public:
    virtual void clear();
    virtual void process(const float *input, uint32_t numFrames) = 0;

    const float *getFrequencies() const { return _freqs.data(); }
    float *getFrequencies() { return _freqs.data(); }
    const float *getMagnitudes() const { return _mags.data(); }
    float *getMagnitudes() { return _mags.data(); }
    uint32_t getNumBins() const { return _numBins; }

private:
    uint32_t _numBins = 0;
    std::vector<float> _freqs;
    std::vector<float> _mags;
};

///
class SteppingAnalyzer : public BasicAnalyzer {
public:
    SteppingAnalyzer();
    virtual ~SteppingAnalyzer() {}

public:
    uint32_t getWindowSize() const { return _windowSize; }

protected:
    void configureStepping(uint32_t numBins, const Configuration &config);

public:
    void setAttackAndRelease(float attack, float release);

    virtual void configure(const Configuration &config) = 0;
    virtual void clear() override;
    virtual void process(const float *input, uint32_t numFrames) override;

protected:
    virtual void processNewBlock(float *input) = 0;

private:
    // window
    std::vector<float> _window;
    uint32_t _windowSize {};

    // analysis step
    uint32_t _stepCounter {};
    uint32_t _stepSize {};

    // input sample accumulation
    uint32_t _ringIndex {};
    std::vector<float> _ring;

    // temporary
    std::vector<float> _input;

    // step-by-step smoother
    class Smoother {
    public:
        void configure(uint32_t numBins, uint32_t stepSize, double attackTime, double releaseTime, double sampleRate);
        void setAttackAndRelease(float attack, float release);
        void clear();
        void process(float *stepData);
    private:
        std::vector<ARFollower> _ar;
        uint32_t _stepSize = 0;
    };
    Smoother _smoother;
};
