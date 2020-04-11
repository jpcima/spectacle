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
    void toggleFreeze();
    bool isFrozen() const { return fFreeze; }

    void setKeyScale(float keyMin, float keyMax);
    void setDbScale(float dbMin, float dbMax);
    void setDefaultScales();

    void onDisplay() override;

public:
    // coordinate <-> unit conversion
    double keyOfX(double x) const;
    double keyOfR(double r) const;
    double frequencyOfX(double x) const;
    double frequencyOfR(double r) const;
    double rOfFrequency(double f) const;
    double xOfFrequency(double f) const;
    double dbMagOfY(double y) const;
    double dbMagOfR(double r) const;
    double rOfDbMag(double m) const;
    double yOfDbMag(double m) const;

private:
    void displayBack();

private:
    FontEngine &fFontEngine;

    std::vector<float> fFrequencies;
    std::vector<float> fMagnitudes;
    uint32_t fSize = 0;
    uint32_t fNumChannels = 0;

    // freeze mode
    bool fFreeze = false;
    std::vector<float> fFreezeFrequencies;
    std::vector<float> fFreezeMagnitudes;
    uint32_t fFreezeSize = 0;
    uint32_t fFreezeNumChannels = 0;

    // scale defaults
    static constexpr float kdBminDefault = -120.0;
    static constexpr float kdBmaxDefault = +0.0;
    static constexpr float kKeyMinDefault = 24.0;
    static constexpr float kKeyMaxDefault = 127.0;

    // vertical scale (dB)
    float fdBmin = kdBminDefault;
    float fdBmax = kdBmaxDefault;

    // horizontal scale (key)
    float fKeyMin = kKeyMinDefault;
    float fKeyMax = kKeyMaxDefault;

    // interpolation
    Spline fSpline;
};
