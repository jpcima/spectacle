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
#include "ui/components/SelectionRectangle.h"
#include "ui/FontEngine.h"
#include "ui/KnobSkin.hpp"
#include "dsp/AnalyzerDefs.h"
#include "util/format_string.h"
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
    kToolBarIdSelect,
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
    fMainToolBar->addButton(kToolBarIdSelect, "Select", "\uf05b");
    fMainToolBar->setListener(this);

    fSkinKnob.reset(new KnobSkin(knobPng, sizeof(knobPng), 31));

    Font fontAwesome;
    fontAwesome.name = "awesome";
    fontAwesome.size = 16.0;
    fontAwesome.color = {0xff, 0xff, 0xff, 0xff};
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

    fScaleWindow = makeSubwidget<FloatingWindow>(this);
    fScaleWindow->setVisible(false);
    fScaleWindow->setSize(150, 70);
    {
        int y = 10;

        TextLabel *label;

        label = makeSubwidget<TextLabel>(fScaleWindow, *fe);
        label->setText("\uf8cc");
        label->setFont(fontAwesome);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fScaleWindow->moveAlong(label);

        label = makeSubwidget<TextLabel>(fScaleWindow, *fe);
        label->setText("left: select zoom region");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(30, y);
        label->setSize(100, 20);
        fScaleWindow->moveAlong(label);

        y += 30;

        label = makeSubwidget<TextLabel>(fScaleWindow, *fe);
        label->setText("\uf8cc");
        label->setFont(fontAwesome);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fScaleWindow->moveAlong(label);

        label = makeSubwidget<TextLabel>(fScaleWindow, *fe);
        label->setText("right: reset zoom");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(30, y);
        label->setSize(100, 20);
        fScaleWindow->moveAlong(label);
    }

    fSelectWindow = makeSubwidget<FloatingWindow>(this);
    fSelectWindow->setVisible(false);
    fSelectWindow->setSize(150, 70);
    {
        int y = 10;

        TextLabel *label;

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        label->setText("\uf337");
        label->setFont(fontAwesome);
        label->setAlignment(kAlignCenter|kAlignInside);
        label->setAbsolutePos(5, y);
        label->setSize(20, 20);
        fSelectWindow->moveAlong(label);

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        fSelectLabelX = label;
        //label->setText("");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(30, y);
        label->setSize(100, 20);
        fSelectWindow->moveAlong(label);

        y += 30;

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        label->setText("\uf338");
        label->setFont(fontAwesome);
        label->setAlignment(kAlignCenter|kAlignInside);
        label->setAbsolutePos(5, y);
        label->setSize(20, 20);
        fSelectWindow->moveAlong(label);

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        fSelectLabelY = label;
        //label->setText("");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(30, y);
        label->setSize(100, 20);
        fSelectWindow->moveAlong(label);
    }

    fSelectionRectangle = makeSubwidget<SelectionRectangle>(this);
    fSelectionRectangle->setVisible(false);

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

bool UISpectralAnalyzer::onMouse(const MouseEvent &ev)
{
    if (fMode == kModeScale && ev.press && ev.button == 1) {
        fSelectionRectangle->setVisible(true);
        fSelectionRectangle->setAbsolutePos(ev.pos);
        fSelectionRectangle->setSize(0, 0);
        fScaleRectDragging = true;
        return true;
    }
    if (fMode == kModeScale && fScaleRectDragging && !ev.press && ev.button == 1) {
        fSelectionRectangle->setVisible(false);
        fScaleRectDragging = false;
        int x1 = fSelectionRectangle->getAbsoluteX() - fSpectrumView->getAbsoluteX();
        int x2 = x1 + (int)fSelectionRectangle->getWidth();
        int y1 = fSelectionRectangle->getAbsoluteY() - fSpectrumView->getAbsoluteY();
        int y2 = y1 + (int)fSelectionRectangle->getHeight();
        if (x1 != x2 && y1 != y2) {
            double key1 = fSpectrumView->keyOfX(x1);
            double key2 = fSpectrumView->keyOfX(x2);
            double db1 = fSpectrumView->dbMagOfY(y1);
            double db2 = fSpectrumView->dbMagOfY(y2);
            if (key1 > key2)
                std::swap(key1, key2);
            if (db1 > db2)
                std::swap(db1, db2);
            fSpectrumView->setKeyScale(key1, key2);
            fSpectrumView->setDbScale(db1, db2);
        }
        return true;
    }
    if (fMode == kModeScale && ev.press && ev.button == 3) {
        fSpectrumView->setDefaultScales();
        return true;
    }

    return false;
}

bool UISpectralAnalyzer::onMotion(const MotionEvent &ev)
{
    switch (fMode) {
    case kModeScale:
        if (fScaleRectDragging) {
            fSelectionRectangle->setSize(
                ev.pos.getX() - fSelectionRectangle->getAbsoluteX(),
                ev.pos.getY() - fSelectionRectangle->getAbsoluteY());
        }
        break;
    case kModeSelect:
        {
            const double key = fSpectrumView->keyOfX(ev.pos.getX() - fSpectrumView->getAbsoluteX());
            const double mag = fSpectrumView->dbMagOfY(ev.pos.getY() - fSpectrumView->getAbsoluteY());
            const double freq = 440.0 * std::exp2((key - 69.0) * (1.0 / 12.0));
            fSpectrumView->setReferenceLine(key, mag);
            fSelectLabelX->setText(format_string("%g", freq) + " Hz");
            fSelectLabelY->setText(format_string("%g", mag) + " dB");
        }
        break;
    }

    return false;
}

// -----------------------------------------------------------------------

void UISpectralAnalyzer::onToolBarItemClicked(int id)
{
    switch (id) {
    case kToolBarIdSetup:
        switchMode((fMode == kModeSetup) ? kModeNormal : kModeSetup);
        break;
    case kToolBarIdScale:
        switchMode((fMode == kModeScale) ? kModeNormal : kModeScale);
        break;
    case kToolBarIdFreeze:
        fSpectrumView->toggleFreeze();
        fMainToolBar->setSelected(kToolBarIdFreeze, fSpectrumView->isFrozen());
        break;
    case kToolBarIdSelect:
        switchMode((fMode == kModeSelect) ? kModeNormal : kModeSelect);
        break;
    }
}

// -----------------------------------------------------------------------

void UISpectralAnalyzer::switchMode(int mode)
{
    int prevMode = fMode;
    if (mode == prevMode)
        return;

    fMode = mode;

    fSetupWindow->setVisible(false);
    fScaleWindow->setVisible(false);
    fSelectWindow->setVisible(false);
    fMainToolBar->setSelected(kToolBarIdSetup, false);
    fMainToolBar->setSelected(kToolBarIdScale, false);
    fMainToolBar->setSelected(kToolBarIdSelect, false);

    const int floatingPosX = 4;
    const int floatingPosY = fMainToolBar->getAbsoluteY() + fMainToolBar->getHeight() + 4;

    switch (prevMode) {
    case kModeScale:
        fScaleRectDragging = false;
        break;
    case kModeSelect:
        fSpectrumView->clearReferenceLine();
        break;
    }

    switch (mode) {
    case kModeSetup:
        fSetupWindow->setVisible(true);
        fSetupWindow->setAbsolutePos(floatingPosX, floatingPosY);
        fMainToolBar->setSelected(kToolBarIdSetup, fMode == kModeSetup);
        break;
    case kModeScale:
        fScaleWindow->setVisible(true);
        fScaleWindow->setAbsolutePos(floatingPosX, floatingPosY);
        fMainToolBar->setSelected(kToolBarIdScale, fMode == kModeScale);
        fScaleRectDragging = false;
        break;
    case kModeSelect:
        fSelectWindow->setVisible(true);
        fSelectWindow->setAbsolutePos(floatingPosX, floatingPosY);
        fMainToolBar->setSelected(kToolBarIdSelect, fMode == kModeSelect);
        fSpectrumView->clearReferenceLine();
        break;
    }
}

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
