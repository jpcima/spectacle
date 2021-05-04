#pragma once
#include "NanoVG.hpp"
#include <string>
#include <functional>
class ColorPalette;

class Slider : public NanoWidget {
public:
    Slider(Widget *group, const ColorPalette &palette);

    double value() const noexcept { return fValue; }
    void setValue(double value);

    bool valueNotified() const { return fValueNotify; }
    void setValueNotified(bool notified);

    void setValueBounds(double v1, double v2);
    void setNumSteps(unsigned numSteps);

    bool onMouse(const MouseEvent &event) override;
    bool onMotion(const MotionEvent &event) override;
    bool onScroll(const ScrollEvent &event) override;
    void onNanoDisplay() override;

    std::function<void(double)> ValueChangedCallback;
    std::function<std::string(double)> FormatCallback;

private:
    double clampToBounds(double value);

private:
    const ColorPalette &fPalette;
    double fValue = 0;
    double fValueBound1 = 0, fValueBound2 = 1;
    unsigned fNumSteps = 100;
    bool fValueNotify = true;
    bool fIsDragging = false;
};
