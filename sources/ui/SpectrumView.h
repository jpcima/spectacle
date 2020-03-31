#pragma once
#include "Widget.hpp"
#include "spline/spline.h"
#include <vector>
#include <complex>
#include <memory>
class FontEngine;

class SpectrumView : public Widget {
public:
    SpectrumView(Widget *parent, FontEngine &fontEngine);

    void setData(const float *frequencies, const float *magnitudes, uint32_t size, uint32_t numChannels);

    void onDisplay() override;

private:
    void displayBack();

    double frequencyOfX(double x) const;
    double frequencyOfR(double r) const;
    double rOfFrequency(double f) const;
    double xOfFrequency(double f) const;

    double rOfDbMag(double m) const;
    double yOfDbMag(double m) const;

private:
    FontEngine &fFontEngine;

    std::vector<float> fFrequencies;
    std::vector<float> fMagnitudes;
    uint32_t fSize = 0;
    uint32_t fNumChannels = 0;

    // vertical scale (dB)
    float fdBmin = -120.0;
    float fdBmax = +0.0;

    // horizontal scale (key)
    int32_t fKeyMin = 24.0f;
    int32_t fKeyMax = 127.0f;

    // interpolation
    Spline fSpline;
};
