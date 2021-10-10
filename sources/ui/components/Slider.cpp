#include "Slider.h"
#include "Window.hpp"
#include "ui/FontEngine.h"
#include "ui/Geometry.h"
#include "ui/NanoVGHelpers.h"
#include "plugin/ColorPalette.h"

///
Slider::Slider(Widget *group, const ColorPalette &palette)
    : NanoWidget(group),
      fPalette(palette)
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
    DGL::Point<int> mpos{int(event.pos.getX() + 0.5), int(event.pos.getY() + 0.5)};

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
    DGL::Point<int> mpos{int(event.pos.getX() + 0.5), int(event.pos.getY() + 0.5)};

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
    DGL::Point<int> mpos{int(event.pos.getX() + 0.5), int(event.pos.getY() + 0.5)};

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

void Slider::onNanoDisplay()
{
    const ColorPalette &cp = fPalette;
    FontEngine fe(*this, cp);

    //
    save();

    //
    const int w = getWidth();
    const int h = getHeight();

    //
    const double v1 = fValueBound1;
    const double v2 = fValueBound2;

    //
    const double value = fValue;
    double fillValue = 0;
    if (v1 != v2)
        fillValue = (value - v1) / (v2 - v1);

    //
    const Rect bounds(0, 0, w, h);

    beginPath();
    ::roundedRect(*this, bounds, 10.0);
    fillColor(Colors::fromRGBA8(cp[Colors::slider_back]));
    fill();

    scissor(0.0, 0.0, (int)(fillValue * w), h);

    beginPath();
    ::roundedRect(*this, bounds, 10.0);
    fillColor(Colors::fromRGBA8(cp[Colors::slider_fill]));
    fill();

    resetScissor();

    ///
    Font fontRegular;
    fontRegular.name = "regular";
    fontRegular.size = 12.0;
    fontRegular.color = cp[Colors::text_normal];

    std::string text;
    if (FormatCallback)
        text = FormatCallback(value);
    else
        text = std::to_string(value);

    fe.drawInBox(text.c_str(), fontRegular, bounds, kAlignCenter|kAlignInside);

    //
    restore();
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
