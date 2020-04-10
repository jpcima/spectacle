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
#include <vector>
#include <memory>
class FontEngine;
class SpectrumView;

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

protected:
    void onToolBarItemClicked(int id) override;

private:
    void updateSpectrum();

private:
    std::unique_ptr<FontEngine> fFontEngine;
    std::unique_ptr<SpectrumView> fSpectrumView;
    std::unique_ptr<MainToolBar> fMainToolBar;

    enum { kNumChannels = DISTRHO_PLUGIN_NUM_INPUTS };

    std::vector<float> fFrequencies;
    std::vector<float> fMagnitudes;
    uint32_t fSize = 0;

private:
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UISpectralAnalyzer)
};
