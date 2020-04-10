#include "Slider.h"
#include "Window.hpp"
#include "Cairo.hpp"
#include "ui/FontEngine.h"
#include "ui/Geometry.h"
#include "ui/Cairo++.h"

///
Slider::Slider(Widget *group, FontEngine &fontEngine)
    : Widget(group),
      fFontEngine(fontEngine)
{
}

void Slider::setValue(double value)
{
    value = clampToBounds(value);

    if (fValue == value)
        return;

    fValue = value;
    if (ValueChangedCallback && fValueNotify)
        ValueChangedCallback(value);
    repaint();
}

void Slider::setValueNotified(bool notified)
{
    fValueNotify = notified;
}

void Slider::setValueBounds(double v1, double v2)
{
    fValueBound1 = v1;
    fValueBound2 = v2;
    setValue(fValue);
}

void Slider::setNumSteps(unsigned numSteps)
{
    fNumSteps = numSteps;
}

bool Slider::onMouse(const MouseEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    if (!fIsDragging && event.press && event.button == 1) {
        bool insideX = mpos.getX() >= 0 && (unsigned)mpos.getX() < wsize.getWidth();
        bool insideY = mpos.getY() >= 0 && (unsigned)mpos.getY() < wsize.getHeight();

        if (!insideX || !insideY)
            return false;

        fIsDragging = true;

        double fill = mpos.getX() / (double)wsize.getWidth();
        setValue(fValueBound1 + fill * (fValueBound2 - fValueBound1));

        return true;
    }
    else if (fIsDragging && !event.press && event.button == 1) {
        fIsDragging = false;
        return true;
    }

    return false;
}

bool Slider::onMotion(const MotionEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    if (fIsDragging) {
        double fill = mpos.getX() / (double)wsize.getWidth();
        fill = (fill < 0) ? 0 : fill;
        fill = (fill > 1) ? 1 : fill;
        setValue(fValueBound1 + fill * (fValueBound2 - fValueBound1));
        return true;
    }

    return false;
}

bool Slider::onScroll(const ScrollEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    bool inside =
        mpos.getX() >= 0 && mpos.getY() >= 0 &&
        (unsigned)mpos.getX() < wsize.getWidth() && (unsigned)mpos.getY() < wsize.getHeight();

    if (inside) {
        double amount = event.delta.getX() - event.delta.getY();
        setValue(fValue + amount * (fValueBound2 - fValueBound1) / fNumSteps);
        return true;
    }

    return false;
}

void Slider::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    FontEngine &fe = fFontEngine;

    //
    int w = getWidth();
    int h = getHeight();

    //
    double v1 = fValueBound1;
    double v2 = fValueBound2;

    //
    double value = fValue;
    double fill = 0;
    if (v1 != v2)
        fill = (value - v1) / (v2 - v1);

    //
    const Rect bounds(0, 0, w, h);
    cairo_rounded_rectangle(cr, bounds, 10.0);
    cairo_set_source_rgb(cr, 0.15, 0.15, 0.15);
    cairo_fill(cr);

    cairo_rectangle(cr, Rect{0, 0, (int)(fill * w), h});
    cairo_clip(cr);

    cairo_rounded_rectangle(cr, bounds, 10.0);
    cairo_set_source_rgb(cr, 0.25, 0.25, 0.25);
    cairo_fill(cr);

    cairo_reset_clip(cr);

    ///
    Font fontRegular;
    fontRegular.name = "regular";
    fontRegular.size = 12.0;
    fontRegular.color = {0xff, 0xff, 0xff, 0xff};

    std::string text;
    if (FormatCallback)
        text = FormatCallback(value);
    else
        text = std::to_string(value);

    fe.drawInBox(cr, text.c_str(), fontRegular, bounds, kAlignCenter|kAlignInside);
}

double Slider::clampToBounds(double value)
{
    double vmin = fValueBound1;
    double vmax = fValueBound2;
    if (vmin > vmax)
        std::swap(vmin, vmax);

    value = (value < vmin) ? vmin : value;
    value = (value > vmax) ? vmax : value;
    return value;
}
