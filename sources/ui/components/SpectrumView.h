#pragma once
#include "NanoVG.hpp"
#include "spline/spline.h"
#include <vector>
#include <complex>
#include <memory>
class ColorPalette;

class SpectrumView : public NanoSubWidget {
public:
    SpectrumView(Widget *parent, const ColorPalette &palette);

    void setData(const float *frequencies, const float *magnitudes, uint32_t size, uint32_t numChannels);
    void toggleFreeze();
    bool isFrozen() const { return fFreeze; }
    double evalMagnitudeOnDisplay(uint32_t channel, double frequency) const;
    struct Peak { double frequency; double magnitude; };
    Peak findNearbyPeakOnDisplay(uint32_t channel, double frequency);

    void setKeyScale(float keyMin, float keyMax);
    void setDbScale(float dbMin, float dbMax);
    void setDefaultScales();

    void clearReferenceLine();
    void setReferenceLine(float key, float db);

    void onNanoDisplay() override;

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
    const ColorPalette &fColorPalette;

    struct Memory {
        uint32_t size;
        uint32_t numChannels;
        std::vector<float> frequencies;
        std::vector<float> magnitudes;
        mutable bool dirty;
        mutable std::vector<Spline> lazySpline;
        Spline &getSpline(uint32_t channel) const;
    };

    Memory fActiveMemory {};

    // freeze mode
    bool fFreeze = false;
    Memory fFreezeMemory {};

    // scale defaults
    static constexpr float kdBminDefault = -96.0;
    static constexpr float kdBmaxDefault = +0.0;
    static constexpr float kKeyMinDefault = 12.0;
    static constexpr float kKeyMaxDefault = 136.72627427729668f;

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
};
