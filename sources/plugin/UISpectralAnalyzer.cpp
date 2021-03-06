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
#include "ColorPalette.h"
#include "Config.h"
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
#include "Color.hpp"
#include <sys/stat.h>

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
    kToolBarIdHide,
    kToolBarIdColor,
};

// -----------------------------------------------------------------------
// Init / Deinit

UISpectralAnalyzer::UISpectralAnalyzer()
    : UI(700, 350),
      fPalette(new ColorPalette(ColorPalette::create_default()))
{
    ColorPalette &palette = *fPalette;

    {
        std::unique_ptr<CSimpleIniA> ini = create_theme();
        ini->SetFileComment(
            ";; This document describes the default theme." "\n"
            ";; Important: do not edit this file directly! changes will be lost." "\n"
            ";; If you want to design a theme, duplicate this file with a different name.");
        ColorPalette::save_defaults(*ini, "color", true);
        save_theme("default", *ini);
    }

    fUiConfig = load_configuration("ui");
    if (!fUiConfig)
        fUiConfig = create_configuration();

    CSimpleIniA &uiConfig = *fUiConfig;
    bool updateUiConfig = false;
    if (!uiConfig.GetFileComment()) {
        uiConfig.SetFileComment(";; This is the configuration of the user interface.");
        updateUiConfig = true;
    }
    if (!uiConfig.GetValue("ui", "theme")) {
        uiConfig.SetValue("ui", "theme", "default", "; Identifier of the theme which is active on program startup");
        updateUiConfig = true;
    }
    if (updateUiConfig)
        save_configuration("ui", uiConfig);

    loadTheme(uiConfig.GetValue("ui", "theme", "default"));

    FontEngine *fe = new FontEngine(palette);
    fFontEngine.reset(fe);
    fe->addFont("regular", fontRegular, sizeof(fontRegular));
    fe->addFont("awesome", fontAwesome, sizeof(fontAwesome));

    fSpectrumView = makeSubwidget<SpectrumView>(this, *fe, palette);

    fMainToolBar = makeSubwidget<MainToolBar>(this, *fe, palette);
    fMainToolBar->addButton(kToolBarIdSetup, "Setup", "\uf085");
    fMainToolBar->addButton(kToolBarIdScale, "Scale", "\uf0b2");
    fMainToolBar->addButton(kToolBarIdFreeze, "Freeze", "\uf256");
    fMainToolBar->addButton(kToolBarIdSelect, "Select", "\uf05b");
    fMainToolBar->addButton(kToolBarIdHide, "Hide", "\uf0d0");
    fMainToolBar->addButton(kToolBarIdColor, "Color", "\uf53f");
    fMainToolBar->setListener(this);

    fSkinKnob.reset(new KnobSkin(knobPng, sizeof(knobPng), 31));

    Font fontAwesome;
    fontAwesome.name = "awesome";
    fontAwesome.size = 16.0;
    fontAwesome.colorRef = Colors::text_normal;
    Font fontLabel;
    fontLabel.name = "regular";
    fontLabel.size = 12.0;
    fontLabel.colorRef = Colors::text_normal;
    Font fontChNLabel[kNumChannels];
    Font fontChNAwesome[kNumChannels];
    for (uint32_t c = 0; c < kNumChannels; ++c) {
        fontChNLabel[c] = fontLabel;
        fontChNAwesome[c] = fontAwesome;
        fontChNLabel[c].colorRef = Colors::text_channel1 + c;
        fontChNAwesome[c].colorRef = Colors::text_channel1 + c;
    }

    fSetupWindow = makeSubwidget<FloatingWindow>(this, palette);
    fSetupWindow->setVisible(false);
    fSetupWindow->setSize(260, 130);
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

        fFftSizeChooser = makeSubwidget<SpinBoxChooser>(fSetupWindow, *fe, palette);
        fFftSizeChooser->setSize(150, 20);
        fFftSizeChooser->setAbsolutePos(100, y);
        for (uint32_t sizeLog2 = kStftMinSizeLog2; sizeLog2 <= kStftMaxSizeLog2; ++sizeLog2)
            fFftSizeChooser->addChoice(sizeLog2, std::to_string(1u << sizeLog2).c_str());
        fFftSizeChooser->ValueChangedCallback = [this](int32_t value)
            { setParameterValue(kPidFftSize, value); };
        fSetupWindow->moveAlong(fFftSizeChooser);

        y += 30;

        label = makeSubwidget<TextLabel>(fSetupWindow, *fe);
        label->setText("Step");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fSetupWindow->moveAlong(label);

        fStepSizeChooser = makeSubwidget<SpinBoxChooser>(fSetupWindow, *fe, palette);
        fStepSizeChooser->setSize(150, 20);
        fStepSizeChooser->setAbsolutePos(100, y);
        for (uint32_t stepLog2 = kStftMinStepLog2; stepLog2 <= kStftMaxStepLog2; ++stepLog2)
            fStepSizeChooser->addChoice(stepLog2, std::to_string(1u << stepLog2).c_str());
        fStepSizeChooser->ValueChangedCallback = [this](int32_t value)
            { setParameterValue(kPidStepSize, value); };
        fSetupWindow->moveAlong(fStepSizeChooser);

        y += 30;

        label = makeSubwidget<TextLabel>(fSetupWindow, *fe);
        label->setText("Attack time");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fSetupWindow->moveAlong(label);

        fAttackTimeSlider = makeSubwidget<Slider>(fSetupWindow, *fe, palette);
        fAttackTimeSlider->setSize(150, 20);
        fAttackTimeSlider->setAbsolutePos(100, y);
        fAttackTimeSlider->setValueBounds(kStftMinAttackTime, kStftMaxAttackTime);
        fAttackTimeSlider->ValueChangedCallback = [this](double value)
            { setParameterValue(kPidAttackTime, value); };
        fAttackTimeSlider->FormatCallback = [](double value) -> std::string
            { return std::to_string((int)(value * 1e3)) + " ms"; };
        fSetupWindow->moveAlong(fAttackTimeSlider);

        y += 30;

        label = makeSubwidget<TextLabel>(fSetupWindow, *fe);
        label->setText("Release time");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fSetupWindow->moveAlong(label);

        fReleaseTimeSlider = makeSubwidget<Slider>(fSetupWindow, *fe, palette);
        fReleaseTimeSlider->setSize(150, 20);
        fReleaseTimeSlider->setAbsolutePos(100, y);
        fReleaseTimeSlider->setValueBounds(kStftMinReleaseTime, kStftMaxReleaseTime);
        fReleaseTimeSlider->ValueChangedCallback = [this](double value)
            { setParameterValue(kPidReleaseTime, value); };
        fReleaseTimeSlider->FormatCallback = [](double value) -> std::string
            { return std::to_string((int)(value * 1e3)) + " ms"; };
        fSetupWindow->moveAlong(fReleaseTimeSlider);
    }

    fScaleWindow = makeSubwidget<FloatingWindow>(this, palette);
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

    fSelectWindow = makeSubwidget<FloatingWindow>(this, palette);
    fSelectWindow->setVisible(false);
    fSelectWindow->setSize(420, 100);
    {
        int x = 0;
        int y = 10;

        TextLabel *label;

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        label->setText("Cursor");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(x + 10, y);
        label->setSize(100, 20);
        fSelectWindow->moveAlong(label);

        y += 30;

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        label->setText("\uf337");
        label->setFont(fontAwesome);
        label->setAlignment(kAlignCenter|kAlignInside);
        label->setAbsolutePos(x + 5, y);
        label->setSize(20, 20);
        fSelectWindow->moveAlong(label);

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        fSelectLabelX = label;
        //label->setText("");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(x + 30, y);
        label->setSize(100, 20);
        fSelectWindow->moveAlong(label);

        y += 30;

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        label->setText("\uf338");
        label->setFont(fontAwesome);
        label->setAlignment(kAlignCenter|kAlignInside);
        label->setAbsolutePos(x + 5, y);
        label->setSize(20, 20);
        fSelectWindow->moveAlong(label);

        label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
        fSelectLabelY = label;
        //label->setText("");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(x + 30, y);
        label->setSize(100, 20);
        fSelectWindow->moveAlong(label);

        y = 10;
        x += 100;

        for (unsigned c = 0; c < kNumChannels; ++c) {
            y += 30;

            label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
            label->setText("\uf140");
            label->setFont(fontChNAwesome[c]);
            label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
            label->setAbsolutePos(x + 10, y);
            label->setSize(100, 20);
            fSelectWindow->moveAlong(label);

            label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
            fSelectChannelY[c] = label;
            //label->setText("");
            label->setFont(fontChNLabel[c]);
            label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
            label->setAbsolutePos(x + 30, y);
            label->setSize(100, 20);
            fSelectWindow->moveAlong(label);
        }

        y = 10;
        x += 120;

        for (unsigned c = 0; c < kNumChannels; ++c) {
            y = 10;

            if (c == 0) {
                label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
                label->setText("Nearby peak");
                label->setFont(fontLabel);
                label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
                label->setAbsolutePos(x + 10, y);
                label->setSize(100, 20);
                fSelectWindow->moveAlong(label);
            }

            y += 30;

            label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
            label->setText("\uf140");
            label->setFont(fontChNAwesome[c]);
            label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
            label->setAbsolutePos(x + 10, y);
            label->setSize(100, 20);
            fSelectWindow->moveAlong(label);

            label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
            fSelectNearPeakX[c] = label;
            //label->setText("");
            label->setFont(fontChNLabel[c]);
            label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
            label->setAbsolutePos(x + 30, y);
            label->setSize(100, 20);
            fSelectWindow->moveAlong(label);

            y += 30;

            label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
            label->setText("\uf140");
            label->setFont(fontChNAwesome[c]);
            label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
            label->setAbsolutePos(x + 10, y);
            label->setSize(100, 20);
            fSelectWindow->moveAlong(label);

            label = makeSubwidget<TextLabel>(fSelectWindow, *fe);
            fSelectNearPeakY[c] = label;
            //label->setText("");
            label->setFont(fontChNLabel[c]);
            label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
            label->setAbsolutePos(x + 30, y);
            label->setSize(100, 20);
            fSelectWindow->moveAlong(label);

            x += 100;
        }
    }

    fColorWindow = makeSubwidget<FloatingWindow>(this, palette);
    fColorWindow->setVisible(false);
    fColorWindow->setSize(260, 70);
    {
        int y = 10;

        TextLabel *label;

        label = makeSubwidget<TextLabel>(fColorWindow, *fe);
        label->setText("Theme");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fColorWindow->moveAlong(label);

        fThemeChooser = makeSubwidget<SpinBoxChooser>(fColorWindow, *fe, palette);
        fThemeChooser->setSize(150, 20);
        fThemeChooser->setAbsolutePos(100, y);
        fColorWindow->moveAlong(fThemeChooser);

        y += 30;

        label = makeSubwidget<TextLabel>(fColorWindow, *fe);
        label->setText("Edit mode");
        label->setFont(fontLabel);
        label->setAlignment(kAlignLeft|kAlignCenter|kAlignInside);
        label->setAbsolutePos(10, y);
        label->setSize(100, 20);
        fColorWindow->moveAlong(label);

        fThemeEditChooser = makeSubwidget<SpinBoxChooser>(fColorWindow, *fe, palette);
        fThemeEditChooser->setSize(150, 20);
        fThemeEditChooser->setAbsolutePos(100, y);
        fThemeEditChooser->addChoice(0, "off");
        fThemeEditChooser->addChoice(1, "on");
        fColorWindow->moveAlong(fThemeEditChooser);
    }

    fSelectionRectangle = makeSubwidget<SelectionRectangle>(this, palette);
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
    case kPidStepSize:
        fStepSizeChooser->setValue(value);
        break;
    case kPidAttackTime:
        fAttackTimeSlider->setValue(value);
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

    // under theme developer mode, recheck periodically for edits
    if (fThemeEditChooser->value()) {
        const std::string path = get_theme_file(fCurrentTheme);
        bool update = false;

        struct stat st;
        if (::stat(path.c_str(), &st) == 0) {
            update = fCurrentThemeMtime.tv_sec != st.st_mtim.tv_sec ||
                fCurrentThemeMtime.tv_nsec != st.st_mtim.tv_nsec;
        }

        if (update)
            loadTheme(fCurrentTheme.c_str());
    }
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

    FloatingWindow *floats[] = {fSetupWindow, fScaleWindow, fSelectWindow, fColorWindow};

    if (!fInitializedFloatingWindowPos) {
        const int floatingPosX = 4;
        const int floatingPosY = fMainToolBar->getAbsoluteY() + fMainToolBar->getHeight() + 4;
        for (FloatingWindow *win : floats)
            win->setAbsolutePos(floatingPosX, floatingPosY);
        fInitializedFloatingWindowPos = true;
    }

    for (FloatingWindow *win : floats) {
        win->setMoveLimits(getAbsolutePos(), getSize());
        // TODO(jpc) I'd rather move the window relative to nearest corner
        win->repositionWithinLimits();
    }
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
    else if (fMode == kModeScale && fScaleRectDragging && !ev.press && ev.button == 1) {
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
    else if (fMode == kModeScale && ev.press && ev.button == 3) {
        fSpectrumView->setDefaultScales();
        return true;
    }
    else if (fMode == kModeSelect && ev.press && ev.button == 1) {
        fSelectPositionFixed = !fSelectPositionFixed;
        if (!fSelectPositionFixed)
            setNewSelectionPositionByMouse(ev.pos);
        return true;
    }
    else if (fMode == kModeHide && ev.press && ev.button == 1) {
        switchMode(kModeNormal);
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
        if (!fSelectPositionFixed)
            setNewSelectionPositionByMouse(ev.pos);
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
    case kToolBarIdHide:
        switchMode(kModeHide);
        break;
    case kToolBarIdColor:
        switchMode((fMode == kModeColor) ? kModeNormal : kModeColor);
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

    fSetupWindow->setAllVisible(false);
    fScaleWindow->setAllVisible(false);
    fSelectWindow->setAllVisible(false);
    fColorWindow->setAllVisible(false);
    fMainToolBar->setSelected(kToolBarIdSetup, false);
    fMainToolBar->setSelected(kToolBarIdScale, false);
    fMainToolBar->setSelected(kToolBarIdSelect, false);
    fMainToolBar->setSelected(kToolBarIdColor, false);

    switch (prevMode) {
    case kModeScale:
        fScaleRectDragging = false;
        break;
    case kModeSelect:
        fSpectrumView->clearReferenceLine();
        break;
    case kModeHide:
        fMainToolBar->setVisible(true);
        break;
    }

    switch (mode) {
    case kModeSetup:
        fSetupWindow->setAllVisible(true);
        fMainToolBar->setSelected(kToolBarIdSetup, fMode == kModeSetup);
        break;
    case kModeScale:
        fScaleWindow->setAllVisible(true);
        fMainToolBar->setSelected(kToolBarIdScale, fMode == kModeScale);
        fScaleRectDragging = false;
        break;
    case kModeSelect:
        fSelectWindow->setAllVisible(true);
        fMainToolBar->setSelected(kToolBarIdSelect, fMode == kModeSelect);
        fSelectPositionFixed = false;
        fSelectLastCursorKey = 0.0;
        fSelectLastCursorFreq = 0.0;
        fSelectLastCursorMag = 0.0;
        fSpectrumView->clearReferenceLine();
        break;
    case kModeHide:
        fMainToolBar->setVisible(false);
        break;
    case kModeColor:
        fColorWindow->setAllVisible(true);
        fMainToolBar->setSelected(kToolBarIdColor, fMode == kModeColor);
        reloadThemeList();
        break;
    }
}

void UISpectralAnalyzer::setNewSelectionPositionByMouse(DGL::Point<int> pos)
{
    const double key = fSpectrumView->keyOfX(pos.getX() - fSpectrumView->getAbsoluteX());
    const double mag = fSpectrumView->dbMagOfY(pos.getY() - fSpectrumView->getAbsoluteY());
    const double freq = 440.0 * std::exp2((key - 69.0) * (1.0 / 12.0));
    fSelectLastCursorKey = key;
    fSelectLastCursorFreq = freq;
    fSelectLastCursorMag = mag;
    updateSelectModeDisplays();
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

    if (fMode == kModeSelect)
        updateSelectModeDisplays();
}

void UISpectralAnalyzer::updateSelectModeDisplays()
{
    auto toHzString = [](double hz) -> std::string {
        return format_string("%.2f", hz) + " Hz";
    };
    auto toDbString = [](double dB) -> std::string {
        if (dB > kStftFloorMagnitudeInDB + kNegligibleDB)
            return format_string("%.2f", dB) + " dB";
        else
            return "-\u221e dB";
    };

    fSpectrumView->setReferenceLine(fSelectLastCursorKey, fSelectLastCursorMag);
    fSelectLabelX->setText(toHzString(fSelectLastCursorFreq));
    fSelectLabelY->setText(toDbString(fSelectLastCursorMag));

    for (unsigned c = 0; c < kNumChannels; ++c) {
        double mag = fSpectrumView->evalMagnitudeOnDisplay(c, fSelectLastCursorFreq);
        fSelectChannelY[c]->setText(toDbString(mag));

        SpectrumView::Peak pk = fSpectrumView->findNearbyPeakOnDisplay(c, fSelectLastCursorFreq);
        fSelectNearPeakX[c]->setText(toHzString(pk.frequency));
        fSelectNearPeakY[c]->setText(toDbString(pk.magnitude));
    }
}

void UISpectralAnalyzer::loadTheme(const char *theme)
{
    ColorPalette &palette = *fPalette;
    palette = ColorPalette::create_default();

    std::unique_ptr<CSimpleIniA> ini;
    const std::string path = get_theme_file(theme);

    if (!path.empty()) {
        ini = create_theme();
        if (ini->LoadFile(path.c_str()) != SI_OK)
            ini.reset();
    }

    fCurrentThemeMtime = timespec{};

    if (!ini)
        fprintf(stderr, "Cannot load theme: %s\n", theme);
    else {
        palette.load(*ini, "color");

        struct stat st;
        if (::stat(path.c_str(), &st) == 0)
            fCurrentThemeMtime = st.st_mtim;
    }

    ///
    CSimpleIniA &uiConfig = *fUiConfig;
    const char *themeInConfig = uiConfig.GetValue("ui", "theme");
    if (!themeInConfig || std::strcmp(themeInConfig, theme) != 0) {
        uiConfig.SetValue("ui", "theme", theme, "; Identifier of the theme which is active on program startup", true);
        save_configuration("ui", uiConfig);
    }

    ///
    fCurrentTheme = theme;
    repaint();
}

void UISpectralAnalyzer::reloadThemeList()
{
    fThemeChooser->ValueChangedCallback = {};
    fThemeChooser->clearChoices();
    {
        int32_t index = -1;
        std::vector<std::string> themes = list_themes();
        for (uint32_t i = 0, n = themes.size(); i < n; ++i) {
            fThemeChooser->addChoice(i, themes[i].c_str());
            if (themes[i] == fCurrentTheme)
                index = i;
        }
        if (index != -1)
            fThemeChooser->setValueIndex(index);
    }
    fThemeChooser->ValueChangedCallback = [this](int32_t index) {
        loadTheme(fThemeChooser->textAtIndex(index).c_str());
    };
}

// -----------------------------------------------------------------------

UI *DISTRHO::createUI()
{
    return new UISpectralAnalyzer();
}
