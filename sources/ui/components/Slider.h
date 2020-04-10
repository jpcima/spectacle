#pragma once
#include "Widget.hpp"
#include <functional>
class FontEngine;

class Slider : public Widget {
public:
    Slider(Widget *group, FontEngine &fontEngine);

    double value() const noexcept { return fValue; }
    void setValue(double value);

    bool valueNotified() const { return fValueNotify; }
    void setValueNotified(bool notified);

    void setValueBounds(double v1, double v2);
    void setNumSteps(unsigned numSteps);

    bool onMouse(const MouseEvent &event) override;
    bool onMotion(const MotionEvent &event) override;
    bool onScroll(const ScrollEvent &event) override;
    void onDisplay() override;

    std::function<void(double)> ValueChangedCallback;

private:
    double clampToBounds(double value);

private:
    FontEngine &fFontEngine;
    double fValue = 0;
    double fValueBound1 = 0, fValueBound2 = 1;
    unsigned fNumSteps = 100;
    bool fValueNotify = true;
    bool fIsDragging = false;
};
