#include "SelectionRectangle.h"
#include "ui/Cairo++.h"
#include "plugin/ColorPalette.h"
#include "Window.hpp"
#include "Cairo.hpp"

SelectionRectangle::SelectionRectangle(Widget *group, ColorPalette &palette)
    : Widget(group),
      fPalette(palette)
{
}

void SelectionRectangle::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    ColorPalette &cp = fPalette;

    int w = getWidth();
    int h = getHeight();

    cairo_rectangle(cr, 0.5, 0.5, w, h);
    cairo_set_source_rgba8(cr, cp[Colors::selection_rectangle]);
    cairo_set_line_width(cr, 1.0);
    const double dash = 4.0;
    cairo_set_dash(cr, &dash, 1, 0.0);
    cairo_stroke(cr);
    cairo_set_dash(cr, nullptr, 0, 0.0);
}
