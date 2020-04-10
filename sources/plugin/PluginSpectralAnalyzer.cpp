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
#include "dsp/AnalyzerDefs.h"
#include "blink/DenormalDisabler.h"
#include <memory>
#include <cstring>

PluginSpectralAnalyzer::PluginSpectralAnalyzer()
    : Plugin(0, 0, 0)
{
    sampleRateChanged(getSampleRate());
}

PluginSpectralAnalyzer::~PluginSpectralAnalyzer()
{
}

// -----------------------------------------------------------------------
// Init

void PluginSpectralAnalyzer::initParameter(uint32_t index, Parameter &parameter)
{
    (void)index;
    (void)parameter;
    DISTRHO_SAFE_ASSERT(false);
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

    std::unique_lock<std::mutex> lock(fSendMutex);
    constexpr uint32_t specMaxSize = kStftMaxSize / 2 + 1;
    fSendFrequencies.resize(kNumChannels * specMaxSize);
    fSendMagnitudes.resize(kNumChannels * specMaxSize);
    lock.unlock();

    const uint32_t fftSize = kStftDefaultSize;
    const uint32_t stepSize = kStftStepSize;
    for (STFT &stft : fStft)
        stft.configure(fftSize, stepSize, kStftSmoothTime, newSampleRate);
}

/**
  Get the current value of a parameter.
*/
float PluginSpectralAnalyzer::getParameterValue(uint32_t index) const
{
    (void)index;
    DISTRHO_SAFE_ASSERT(false);
    return 0;
}

/**
  Change a parameter value.
*/
void PluginSpectralAnalyzer::setParameterValue(uint32_t index, float value)
{
    (void)index;
    (void)value;
    DISTRHO_SAFE_ASSERT(false);
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
    for (STFT &stft : fStft)
        stft.clear();
}

void PluginSpectralAnalyzer::run(const float **inputs, float **outputs, uint32_t frames)
{
    WebCore::DenormalDisabler dd;

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
