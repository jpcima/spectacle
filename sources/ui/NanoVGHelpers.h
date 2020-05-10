#pragma once
#include "ui/Geometry.h"
#include "NanoVG.hpp"
#include <cmath>

enum RectangleCorner {
    RectangleNE = 1 << 0,
    RectangleSE = 1 << 1,
    RectangleSW = 1 << 2,
    RectangleNW = 1 << 3,
    RectangleAllCorners = RectangleNE|RectangleSE|RectangleSW|RectangleNW,
};

template <class T>
void rect(NanoVG &vg, const RectT<T> &r)
{
    vg.rect(r.x, r.y, r.w, r.h);
}

template <class T>
void roundedRect(NanoVG &vg, const RectT<T> &r, double radius)
{
    vg.roundedRect(r.x, r.y, r.w, r.h, radius);
}

template <class T>
void roundedRectWithCorners(NanoVG &vg, const RectT<T> &r, double radius, int corners)
{
    double x = r.x;
    double y = r.y;
    double w = r.w;
    double h = r.h;

    if (corners & RectangleNE) {
        vg.moveTo(x + w - radius, y);
        vg.arcTo(x + w, y, x + w, y + radius, radius);
    }
    else {
        vg.moveTo(x + w, y);
    }

    if (corners & RectangleSE) {
        vg.lineTo(x + w, y + h - radius);
        vg.arcTo(x + w, y + h, x + w - radius, y + h , radius);
    }
    else {
        vg.lineTo(x + w, y + h);
    }

    if (corners & RectangleSW) {
        vg.lineTo(x + radius, y + h);
        vg.arcTo(x, y + h, x, y + h - radius, radius);
    }
    else {
        vg.lineTo(x, y + h);
    }

    if (corners & RectangleNW) {
        vg.lineTo(x, y + radius);
        vg.arcTo(x, y, x + radius, y, radius);
    }
    else {
        vg.lineTo(x, y);
    }

    if (corners & RectangleNE)
        vg.lineTo(x + w - radius, y);
    else
        vg.lineTo(x + w, y);
}
