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
#include "Parameters.h"
#include "ui/components/SpectrumView.h"
#include "ui/components/FloatingWindow.h"
#include "ui/components/SkinSlider.hpp"
#include "ui/components/SpinBoxChooser.h"
#include "ui/components/Slider.h"
#include "ui/components/TextLabel.h"
#include "ui/FontEngine.h"
#include "ui/KnobSkin.hpp"
#include "dsp/AnalyzerDefs.h"
#include "Window.hpp"

static constexpr uint8_t fontRegular[] = {
    #include "fonts/liberation/LiberationSans-Regular.ttf.h"
};
static constexpr uint8_t fontAwesome[] = {
    #include "fonts/fontawesome/Font-Awesome-5-Free-Solid-900.otf.h"
};
static constexpr uint8_t knobPng[] = {
    #include "skin/knob.png.h"
};

enum {
    kToolBarIdSetup = 1,
    kToolBarIdScale,
    kToolBarIdFreeze,
};

// -----------------------------------------------------------------------
// Init / Deinit

UISpectralAnalyzer::UISpectralAnalyzer()
    : UI(1200, 600)
{
    FontEngine *fe = new FontEngine;
    fFontEngine.reset(fe);
    fe->addFont("regular", fontRegular, sizeof(fontRegular));
    fe->addFont("awesome", fontAwesome, sizeof(fontAwesome));

    fSpectrumView = makeSubwidget<SpectrumView>(this, *fe);

    fMainToolBar = makeSubwidget<MainToolBar>(this, *fe);
    fMainToolBar->addButton(kToolBarIdSetup, "Setup", "\uf085");
    fMainToolBar->addButton(kToolBarIdScale, "Scale", "\uf0b2");
    fMainToolBar->addButton(kToolBarIdFreeze, "Freeze", "\uf256");
    fMainToolBar->setListener(this);

    fSkinKnob.reset(new KnobSkin(knobPng, sizeof(knobPng), 31));

    Font fontLabel;
    fontLabel.name = "regular";
    fontLabel.size = 12.0;
    fontLabel.color = {0xff, 0xff, 0xff, 0xff};

    fSetupWindow = makeSubwidget<FloatingWindow>(this);
    fSetupWindow->setVisible(false);
    fSetupWindow->setSize(260, 70);
    {
        int y = 10;

        TextLabel *label;

        label = makeSubwidget<TextLabel>(fSetupWindow, *fe);
        label->setText("Resolution");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fSetupWindow->moveAlong(label);

        fFftSizeChooser = makeSubwidget<SpinBoxChooser>(fSetupWindow, *fe);
        fFftSizeChooser->setSize(150, 20);
        fFftSizeChooser->setAbsolutePos(100, y);
        for (uint32_t sizeLog2 = kStftMinSizeLog2; sizeLog2 <= kStftMaxSizeLog2; ++sizeLog2)
            fFftSizeChooser->addChoice(sizeLog2, std::to_string(1u << sizeLog2).c_str());
        fFftSizeChooser->ValueChangedCallback = [this](int32_t value)
            { setParameterValue(kPidFftSize, value); };
        fSetupWindow->moveAlong(fFftSizeChooser);

        y += 30;

        label = makeSubwidget<TextLabel>(fSetupWindow, *fe);
        label->setText("Release time");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fSetupWindow->moveAlong(label);

        fReleaseTimeSlider = makeSubwidget<Slider>(fSetupWindow, *fe);
        fReleaseTimeSlider->setSize(150, 20);
        fReleaseTimeSlider->setAbsolutePos(100, y);
        fReleaseTimeSlider->setValueBounds(kStftMinSmoothTime, kStftMaxSmoothTime);
        fReleaseTimeSlider->ValueChangedCallback = [this](double value)
            { setParameterValue(kPidReleaseTime, value); };
        fReleaseTimeSlider->FormatCallback = [](double value) -> std::string
            { return std::to_string((int)(value * 1e3)) + " ms"; };
        fSetupWindow->moveAlong(fReleaseTimeSlider);
    }

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
    switch (index) {
    case kPidFftSize:
        fFftSizeChooser->setValue(value);
        break;
    case kPidReleaseTime:
        fReleaseTimeSlider->setValue(value);
        break;
    }
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

    fMainToolBar->setAbsolutePos(0, 0);
    fMainToolBar->setSize(fMainToolBar->getIdealWidth(), 40);
}

// -----------------------------------------------------------------------

void UISpectralAnalyzer::onDisplay()
{
}

// -----------------------------------------------------------------------

void UISpectralAnalyzer::onToolBarItemClicked(int id)
{
    switch (id) {
    case kToolBarIdSetup:
        if (fSetupWindow->isVisible())
            fSetupWindow->setVisible(false);
        else {
            fSetupWindow->setVisible(true);
            fSetupWindow->setAbsolutePos(100, 100);
        }
        break;
    case kToolBarIdScale:
        //
        break;
    case kToolBarIdFreeze:
        fSpectrumView->toggleFreeze();
        break;
    }
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
