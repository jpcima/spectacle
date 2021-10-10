#include "FloatingWindow.h"
#include "plugin/ColorPalette.h"
#include "Window.hpp"

FloatingWindow::FloatingWindow(Widget *group, ColorPalette &palette)
    : NanoWidget(group),
      fPalette(palette)
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

void FloatingWindow::moveAlong(SubWidget *w)
{
    DISTRHO_SAFE_ASSERT_RETURN(w != nullptr, );

    fMoveAlong.push_back(w);
}

void FloatingWindow::setAllVisible(bool visible)
{
    setVisible(visible);
    for (Widget *w : fMoveAlong)
        w->setVisible(visible);
}

void FloatingWindow::onNanoDisplay()
{
    ColorPalette &cp = fPalette;

    double w = getWidth();
    double h = getHeight();

    beginPath();
    roundedRect(0.0, 0.0, w, h, 10.0);
    fillColor(Colors::fromRGBA8(cp[Colors::floating_window_back]));
    fill();
}

void FloatingWindow::onPositionChanged(const PositionChangedEvent &ev)
{
    int dX = ev.pos.getX() - ev.oldPos.getX();
    int dY = ev.pos.getY() - ev.oldPos.getY();

    for (SubWidget *w : fMoveAlong) {
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

    x = std::max(lx - (int)(w / 2), std::min(lx + (int)lw - (int)(w / 2), x));
    y = std::max(ly - (int)(h / 2), std::min(ly + (int)lh - (int)(h / 2), y));

    return DGL::Point<int>(x, y);
}
