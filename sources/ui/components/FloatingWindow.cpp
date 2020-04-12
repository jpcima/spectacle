#include "FloatingWindow.h"
#include "ui/Cairo++.h"
#include "Window.hpp"
#include "Cairo.hpp"

FloatingWindow::FloatingWindow(Widget *group)
    : Widget(group)
{
}

void FloatingWindow::setMoveLimits(DGL::Point<int> origin, DGL::Size<uint> size)
{
    fLimitOrigin = origin;
    fLimitSize = size;
}

void FloatingWindow::repositionWithinLimits()
{
    setAbsolutePos(restrictWithinLimits(getAbsolutePos()));
}

void FloatingWindow::moveAlong(Widget *w)
{
    DISTRHO_SAFE_ASSERT_RETURN(w != nullptr, );

    fMoveAlong.push_back(w);
}

void FloatingWindow::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;

    double w = getWidth();
    double h = getHeight();

    cairo_rounded_rectangle(cr, RectF{0.0, 0.0, w, h}, 10.0);
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.25);
    cairo_fill(cr);
}

void FloatingWindow::onPositionChanged(const PositionChangedEvent &ev)
{
    int dX = ev.pos.getX() - ev.oldPos.getX();
    int dY = ev.pos.getY() - ev.oldPos.getY();

    for (Widget *w : fMoveAlong) {
        DGL::Point<int> p = w->getAbsolutePos();
        p.setX(p.getX() + dX);
        p.setY(p.getY() + dY);
        w->setAbsolutePos(p);
    }
}

bool FloatingWindow::onMouse(const MouseEvent &ev)
{
    if (ev.press && ev.button == 1) {
        const int w = getWidth();
        const int h = getHeight();
        const int evX = ev.pos.getX();
        const int evY = ev.pos.getY();
        const DGL::Point<int> a = getAbsolutePos();
        const bool inside = evX >= 0 && evX < w && evY >= 0 && evY < h;
        if (inside) {
            fIsDragging = true;
            fDragMouseOrigin = {evX + a.getX(), evY + a.getY()};
            fDragStartingWindowPos = a;
            return true;
        }
    }
    else if (!ev.press && ev.button == 1) {
        if (fIsDragging) {
            fIsDragging = false;
            return true;
        }
    }

    return false;
}

bool FloatingWindow::onMotion(const MotionEvent &ev)
{
    if (fIsDragging) {
        DGL::Point<int> orig = fDragMouseOrigin;
        int curx = ev.pos.getX() + getAbsoluteX();
        int cury = ev.pos.getY() + getAbsoluteY();
        int dx = curx - orig.getX();
        int dy = cury - orig.getY();
        DGL::Point<int> a = fDragStartingWindowPos;
        a += DGL::Point<int>(dx, dy);
        setAbsolutePos(restrictWithinLimits(a));
        return true;
    }

    return false;
}

DGL::Point<int> FloatingWindow::restrictWithinLimits(DGL::Point<int> pos)
{
    int x = pos.getX();
    int y = pos.getY();
    const uint w = getWidth();
    const uint h = getHeight();
    const int lx = fLimitOrigin.getX();
    const int ly = fLimitOrigin.getY();
    const uint lw = fLimitSize.getWidth();
    const uint lh = fLimitSize.getHeight();

    if (lw == 0 || lh == 0)
        return pos;

    x = std::max(lx, std::min(lx + (int)lw - (int)w, x));
    y = std::max(ly, std::min(ly + (int)lh - (int)h, y));

    return DGL::Point<int>(x, y);
}
