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

#pragma once
#include "DistrhoUI.hpp"
#include "PluginSpectralAnalyzer.hpp"
#include "ui/components/MainToolBar.h"
#include "SimpleIni.h"
#include <string>
#include <vector>
#include <memory>
#include <sys/time.h> // timespec
class FontEngine;
class SpectrumView;
class FloatingWindow;
class SpinBoxChooser;
class Slider;
class TextLabel;
class SelectionRectangle;
class KnobSkin;
class ColorPalette;

class UISpectralAnalyzer : public UI, MainToolBar::Listener {
public:
    UISpectralAnalyzer();
    ~UISpectralAnalyzer();

    PluginSpectralAnalyzer *getPluginInstance();

protected:
    void parameterChanged(uint32_t, float value) override;
    void programLoaded(uint32_t index) override;
    void sampleRateChanged(double newSampleRate) override;

    void uiIdle() override;
    void uiReshape(uint width, uint height) override;

    void onDisplay() override;
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

protected:
    void onToolBarItemClicked(int id) override;

private:
    void switchMode(int mode);

    void setNewSelectionPositionByMouse(DGL::Point<int> pos);

    void updateSpectrum();
    void updateSelectModeDisplays();

    template <class W, class... A>
    W *makeSubwidget(A &&... args)
    {
        W *w = new W(std::forward<A>(args)...);
        fSubWidgets.push_back(std::unique_ptr<Widget>(w));
        return w;
    }

    void loadTheme(const char *theme);
    void reloadThemeList();

private:
    enum { kNumChannels = DISTRHO_PLUGIN_NUM_INPUTS };

    std::unique_ptr<FontEngine> fFontEngine;
    SpectrumView *fSpectrumView = nullptr;

    MainToolBar *fMainToolBar = nullptr;

    FloatingWindow *fSetupWindow = nullptr;
    SpinBoxChooser *fFftSizeChooser = nullptr;
    SpinBoxChooser *fStepSizeChooser = nullptr;
    Slider *fAttackTimeSlider = nullptr;
    Slider *fReleaseTimeSlider = nullptr;

    FloatingWindow *fScaleWindow = nullptr;
    SelectionRectangle *fSelectionRectangle = nullptr;

    FloatingWindow *fSelectWindow = nullptr;
    TextLabel *fSelectLabelX = nullptr;
    TextLabel *fSelectLabelY = nullptr;
    TextLabel *fSelectChannelY[kNumChannels] = {};
    TextLabel *fSelectNearPeakX[kNumChannels] = {};
    TextLabel *fSelectNearPeakY[kNumChannels] = {};
    bool fSelectPositionFixed = false;
    double fSelectLastCursorKey = 0.0;
    double fSelectLastCursorFreq = 0.0;
    double fSelectLastCursorMag = 0.0;

    FloatingWindow *fColorWindow = nullptr;
    SpinBoxChooser *fThemeChooser = nullptr;
    SpinBoxChooser *fThemeEditChooser = nullptr;

    std::unique_ptr<KnobSkin> fSkinKnob;

    std::vector<std::unique_ptr<Widget>> fSubWidgets;

    std::vector<float> fFrequencies;
    std::vector<float> fMagnitudes;
    uint32_t fSize = 0;

    enum {
        kModeNormal,
        kModeSetup,
        kModeScale,
        kModeSelect,
        kModeHide,
        kModeColor,
    };

    int fMode = kModeNormal;

    bool fScaleRectDragging = false;

    bool fInitializedFloatingWindowPos = false;

    std::unique_ptr<ColorPalette> fPalette;

    std::unique_ptr<CSimpleIniA> fUiConfig;
    std::string fCurrentTheme = "default";
    timespec fCurrentThemeMtime {};
    bool fThemeEditMode = false;

private:
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UISpectralAnalyzer)
};
