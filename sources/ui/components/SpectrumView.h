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

    void clearReferenceLine();
    void setReferenceLine(float key, float db);

    void onDisplay() override;

public:
    // coordinate <-> unit conversion
    double keyOfX(double x) const;
    double keyOfR(double r) const;
    double xOfKey(double k) const;
    double rOfKey(double k) const;
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
    struct Memory;
    const Memory &getDisplayMemory() const { return fFreeze ? fFreezeMemory : fActiveMemory; }

private:
    FontEngine &fFontEngine;

    struct Memory {
        uint32_t size;
        uint32_t numChannels;
        std::vector<float> frequencies;
        std::vector<float> magnitudes;
    };

    Memory fActiveMemory;

    // freeze mode
    bool fFreeze = false;
    Memory fFreezeMemory;

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

    // reference line
    bool fHaveReferenceLine = false;
    float fKeyRef = 0;
    float fdBref = 0;

    // interpolation
    Spline fSpline;
};
