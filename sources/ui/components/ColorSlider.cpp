#include "ColorSlider.h"
#include "Window.hpp"
#include "Cairo.hpp"
#include "Color.hpp"
#include "ui/Geometry.h"
#include "ui/Cairo++.h"
#include <algorithm>
#include <cstring>

///
ColorSlider::ColorSlider(Widget *group)
    : Widget(group)
{
}

void ColorSlider::setValue(double value)
{
    value = clampToBounds(value);

    if (fValue == value)
        return;

    fValue = value;
    if (ValueChangedCallback && fValueNotify)
        ValueChangedCallback(value);
    repaint();
}

void ColorSlider::setValueNotified(bool notified)
{
    fValueNotify = notified;
}

void ColorSlider::setValueBounds(double v1, double v2)
{
    fValueBound1 = v1;
    fValueBound2 = v2;
    setValue(fValue);
}

void ColorSlider::setNumSteps(unsigned numSteps)
{
    fNumSteps = numSteps;
}

bool ColorSlider::onMouse(const MouseEvent &event)
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

bool ColorSlider::onMotion(const MotionEvent &event)
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

bool ColorSlider::onScroll(const ScrollEvent &event)
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

void ColorSlider::onResize(const ResizeEvent &event)
{
    (void)event;
    fBackImage.reset();
}

void ColorSlider::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    cairo_save(cr);

    //
    const int w = getWidth();
    const int h = getHeight();

    const int tsize = 12.0; // height of triangle mark
    const int barh = h - tsize / 2;

    //
    double v1 = fValueBound1;
    double v2 = fValueBound2;

    //
    double value = fValue;
    double fill = 0;
    if (v1 != v2)
        fill = (value - v1) / (v2 - v1);

    cairo_surface_t *back = fBackImage.get();
    if (!back) {
        back = cairo_surface_create_similar_image(cairo_get_target(cr), CAIRO_FORMAT_ARGB32, w, barh);
        fBackImage.reset(back);
        paintBackground(back);
    }

    cairo_set_line_width(cr, 0.5);

    //
    cairo_rectangle(cr, Rect(0, 0, w, barh));
    cairo_set_source_surface(cr, back, 0.0, 0.0);
    cairo_fill_preserve(cr);
    cairo_rectangle(cr, RectF(0.5, 0.5, w, barh));
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_stroke(cr);

    //
    cairo_new_path(cr);
    double tx = fill * w;
    double ty = h - tsize;
    cairo_move_to(cr, tx, ty);
    cairo_line_to(cr, tx - 0.5 * tsize, ty + tsize);
    cairo_line_to(cr, tx + 0.5 * tsize, ty + tsize);
    cairo_close_path(cr);
    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
    cairo_fill_preserve(cr);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_stroke(cr);

    //
    cairo_restore(cr);
}

void ColorSlider::paintBackground(cairo_surface_t *image)
{
    DISTRHO_SAFE_ASSERT_RETURN(cairo_image_surface_get_format(image) == CAIRO_FORMAT_ARGB32, );

    const int w = cairo_image_surface_get_width(image);
    const int h = cairo_image_surface_get_height(image);

    cairo_surface_flush(image);
    uint8_t *data = cairo_image_surface_get_data(image);
    int stride = cairo_image_surface_get_stride(image);
    for (int x = 0; x < w; ++x) {
        Color c = Color::fromHSL(x * (1.0f / w), 0.5, 0.5);
        int r = std::max(0x00, std::min(0xff, int(c.red * 0xff)));
        int g = std::max(0x00, std::min(0xff, int(c.green * 0xff)));
        int b = std::max(0x00, std::min(0xff, int(c.blue * 0xff)));
        *(uint32_t *)&data[4 * x] = (0xff << 24) | (r << 16) | (g << 8) | b;
    }

    for (int y = 1; y < h; ++y)
        std::memcpy(data + y * stride, data, 4 * w);

    cairo_surface_mark_dirty(image);
}

double ColorSlider::clampToBounds(double value)
{
    double vmin = fValueBound1;
    double vmax = fValueBound2;
    if (vmin > vmax)
        std::swap(vmin, vmax);

    value = (value < vmin) ? vmin : value;
    value = (value > vmax) ? vmax : value;
    return value;
}
