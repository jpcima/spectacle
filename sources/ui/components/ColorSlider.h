#pragma once
#include "Widget.hpp"
#include "ui/Cairo++.h"
#include <string>
#include <functional>

class ColorSlider : public Widget {
public:
    explicit ColorSlider(Widget *group);

    double value() const noexcept { return fValue; }
    void setValue(double value);

    bool valueNotified() const { return fValueNotify; }
    void setValueNotified(bool notified);

    void setValueBounds(double v1, double v2);
    void setNumSteps(unsigned numSteps);

    bool onMouse(const MouseEvent &event) override;
    bool onMotion(const MotionEvent &event) override;
    bool onScroll(const ScrollEvent &event) override;
    void onResize(const ResizeEvent &event) override;
    void onDisplay() override;

    std::function<void(double)> ValueChangedCallback;

private:
    static void paintBackground(cairo_surface_t *image);

private:
    double clampToBounds(double value);

private:
    double fValue = 0;
    double fValueBound1 = 0, fValueBound2 = 1;
    unsigned fNumSteps = 100;
    bool fValueNotify = true;
    bool fIsDragging = false;
    cairo_surface_u fBackImage;
};
