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

#include "UISpectralAnalyzer.hpp"
#include "ui/FontEngine.h"
#include "ui/SpectrumView.h"
#include "Window.hpp"

static constexpr uint8_t fontRegular[] = {
    #include "fonts/liberation/LiberationSans-Regular.ttf.h"
};

// -----------------------------------------------------------------------
// Init / Deinit

UISpectralAnalyzer::UISpectralAnalyzer()
    : UI(1200, 600)
{
    FontEngine *fe = new FontEngine;
    fFontEngine.reset(fe);
    fe->addFont("regular", fontRegular, sizeof(fontRegular));

    SpectrumView *sv = new SpectrumView(this, *fe);
    fSpectrumView.reset(sv);

    uiReshape(getWidth(), getHeight());
}

UISpectralAnalyzer::~UISpectralAnalyzer()
{
}

PluginSpectralAnalyzer *UISpectralAnalyzer::getPluginInstance()
{
    return static_cast<PluginSpectralAnalyzer *>(getPluginInstancePointer());
}

// -----------------------------------------------------------------------
// DSP/Plugin callbacks

/**
  A parameter has changed on the plugin side.
  This is called by the host to inform the UI about parameter changes.
*/
void UISpectralAnalyzer::parameterChanged(uint32_t index, float value)
{
    (void)index;
    (void)value;
    DISTRHO_SAFE_ASSERT(false);
}

/**
  A program has been loaded on the plugin side.
  This is called by the host to inform the UI about program changes.
*/
void UISpectralAnalyzer::programLoaded(uint32_t index)
{
    (void)index;
    DISTRHO_SAFE_ASSERT(false);
}

/**
  Optional callback to inform the UI about a sample rate change on the plugin side.
*/
void UISpectralAnalyzer::sampleRateChanged(double newSampleRate)
{
    (void)newSampleRate;
}

// -----------------------------------------------------------------------
// Optional UI callbacks

/**
  Idle callback.
  This function is called at regular intervals.
*/
void UISpectralAnalyzer::uiIdle()
{
    updateSpectrum();
}

/**
  Window reshape function, called when the parent window is resized.
*/
void UISpectralAnalyzer::uiReshape(uint width, uint height)
{
    fSpectrumView->setAbsolutePos(0, 0);
    fSpectrumView->setSize(width, height);
}

// -----------------------------------------------------------------------

void UISpectralAnalyzer::onDisplay()
{
}

// -----------------------------------------------------------------------

void UISpectralAnalyzer::updateSpectrum()
{
    PluginSpectralAnalyzer *plugin = getPluginInstance();
    DISTRHO_SAFE_ASSERT_RETURN(plugin, );

    std::unique_lock<std::mutex> lock(plugin->fSendMutex);
    fFrequencies = plugin->fSendFrequencies;
    fMagnitudes = plugin->fSendMagnitudes;
    fSize = plugin->fSendSize;
    lock.unlock();

    fSpectrumView->setData(fFrequencies.data(), fMagnitudes.data(), fSize, kNumChannels);
}

// -----------------------------------------------------------------------

UI *DISTRHO::createUI()
{
    return new UISpectralAnalyzer();
}
