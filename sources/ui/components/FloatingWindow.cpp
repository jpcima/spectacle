#include "FloatingWindow.h"
#include "ui/Cairo++.h"
#include "Window.hpp"
#include "Cairo.hpp"

FloatingWindow::FloatingWindow(Widget *group)
    : Widget(group)
{
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
        setAbsolutePos(a.getX() + dx, a.getY() + dy);
        return true;
    }

    return false;
}
