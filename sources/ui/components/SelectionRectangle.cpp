#include "SelectionRectangle.h"
#include "Window.hpp"
#include "Cairo.hpp"

SelectionRectangle::SelectionRectangle(Widget *group)
    : Widget(group)
{
}

void SelectionRectangle::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;

    int w = getWidth();
    int h = getHeight();

    cairo_rectangle(cr, 0.5, 0.5, w, h);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_set_line_width(cr, 1.0);
    const double dash = 4.0;
    cairo_set_dash(cr, &dash, 1, 0.0);
    cairo_stroke(cr);
    cairo_set_dash(cr, nullptr, 0, 0.0);
}
