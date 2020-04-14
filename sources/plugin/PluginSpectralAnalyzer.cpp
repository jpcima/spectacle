/*
 * Spectral Analyzer audio effect based on DISTRHO Plugin Framework (DPF)
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2019 Jean Pierre Cimalando <jpcima@protonmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "PluginSpectralAnalyzer.hpp"
#include "Parameters.h"
#include "dsp/AnalyzerDefs.h"
#include "blink/DenormalDisabler.h"
#include <memory>
#include <cstring>

PluginSpectralAnalyzer::PluginSpectralAnalyzer()
    : Plugin(kParameterCount, 0, 0),
      fParameters(new float[kParameterCount]),
      fParameterRanges(new ParameterRanges[kParameterCount])
{
    for (uint32_t i = 0; i < kParameterCount; ++i) {
        Parameter p;
        InitParameter(i, p);
        fParameters[i] = p.ranges.def;
        fParameterRanges[i] = p.ranges;
    }

    constexpr uint32_t specMaxSize = kStftMaxSize / 2 + 1;
    fSendFrequencies.resize(kNumChannels * specMaxSize);
    fSendMagnitudes.resize(kNumChannels * specMaxSize);

    sampleRateChanged(getSampleRate());
}

PluginSpectralAnalyzer::~PluginSpectralAnalyzer()
{
}

// -----------------------------------------------------------------------
// Init

void PluginSpectralAnalyzer::initParameter(uint32_t index, Parameter &parameter)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < kParameterCount, );
    InitParameter(index, parameter);
}

/**
  Set the name of the program @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginSpectralAnalyzer::initProgramName(uint32_t index, String &programName)
{
    (void)index;
    (void)programName;
    DISTRHO_SAFE_ASSERT(false);
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginSpectralAnalyzer::sampleRateChanged(double newSampleRate)
{
    fSampleRate = newSampleRate;
    fMustReconfigureStft = 1;
}

/**
  Get the current value of a parameter.
*/
float PluginSpectralAnalyzer::getParameterValue(uint32_t index) const
{
    DISTRHO_SAFE_ASSERT_RETURN(index < kParameterCount, 0.0);
    return fParameters[index];
}

/**
  Change a parameter value.
*/
void PluginSpectralAnalyzer::setParameterValue(uint32_t index, float value)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < kParameterCount, );
    fParameterRanges[index].fixValue(value);
    fParameters[index] = value;

    switch (index) {
    case kPidFftSize:
    case kPidReleaseTime:
    case kPidAttackTime:
        fMustReconfigureStft = 1;
        break;
    }
}

/**
  Load a program.
  The host may call this function from any context,
  including realtime processing.
*/
void PluginSpectralAnalyzer::loadProgram(uint32_t index)
{
    (void)index;
    DISTRHO_SAFE_ASSERT(false);
}

// -----------------------------------------------------------------------
// Process

void PluginSpectralAnalyzer::activate()
{
    fMustReconfigureStft = 1;
}

void PluginSpectralAnalyzer::run(const float **inputs, float **outputs, uint32_t frames)
{
    WebCore::DenormalDisabler dd;

    if (fMustReconfigureStft.exchange(0)) {
        const double sampleRate = fSampleRate;
        const uint32_t fftSize = 1u << (uint32_t)fParameters[kPidFftSize];
        const float attackTime = fParameters[kPidAttackTime];
        const float releaseTime = fParameters[kPidReleaseTime];
        for (STFT &stft : fStft)
            stft.configure(fftSize, kStftStepSize, attackTime, releaseTime, sampleRate);
    }

    for (uint32_t c = 0; c < kNumChannels; ++c) {
        STFT &stft = fStft[c];
        const float *input = inputs[c];
        stft.process(input, frames);
    }

    std::unique_lock<std::mutex> sendLock(fSendMutex, std::try_to_lock);
    if (sendLock.owns_lock()) {
        const uint32_t fftSize = fStft[0].getFftSize();
        const uint32_t specSize = fftSize / 2 + 1;
        fSendSize = specSize;
        for (uint32_t c = 0; c < kNumChannels; ++c) {
            STFT &stft = fStft[c];
            std::memcpy(
                &fSendFrequencies[c * specSize], stft.getFrequencies(),
                specSize * sizeof(float));
            std::memcpy(
                &fSendMagnitudes[c * specSize], stft.getMagnitudes(),
                specSize * sizeof(float));
        }
    }

    for (uint32_t c = 0; c < kNumChannels; ++c) {
        if (inputs[c] != outputs[c])
            std::memcpy(outputs[c], inputs[c], frames * sizeof(float));
    }
}

// -----------------------------------------------------------------------

Plugin *DISTRHO::createPlugin()
{
    return new PluginSpectralAnalyzer();
}
