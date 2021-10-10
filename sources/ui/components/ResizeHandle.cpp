#include "ResizeHandle.h"
#include "ui/Geometry.h"
#include "ui/NanoVGHelpers.h"
#include "plugin/ColorPalette.h"
#include "Window.hpp"

ResizeHandle::ResizeHandle(Widget *group, const ColorPalette &palette)
    : NanoWidget(group),
      fPalette(palette)
{
}

bool ResizeHandle::onMouse(const MouseEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos{int(event.pos.getX() + 0.5), int(event.pos.getY() + 0.5)};

    if (!fIsDragging && event.press && event.button == 1) {
        bool insideX = mpos.getX() >= 0 && (unsigned)mpos.getX() < wsize.getWidth();
        bool insideY = mpos.getY() >= 0 && (unsigned)mpos.getY() < wsize.getHeight();

        if (!insideX || !insideY)
            return false;

        fIsDragging = true;

        return true;
    }
    else if (fIsDragging && !event.press && event.button == 1) {
        fIsDragging = false;
        return true;
    }

    return false;
}

bool ResizeHandle::onMotion(const MotionEvent &event)
{
    if (!fIsDragging)
        return false;

    DGL::Point<int> mpos{int(event.pos.getX() + 0.5), int(event.pos.getY() + 0.5)};

    // same as how wolf-common does it
    DGL::Point<int> size((int)getWidth(), (int)getHeight());
    DGL::Point<int> center(size.getX() / 2, size.getY() / 2);
    DGL::Point<int> translation = mpos - center;
    DGL::Point<int> absPos = getAbsolutePos();
    DGL::Point<int> topLeft = absPos + translation;
    DGL::Point<int> bottomRight = topLeft + size;

    DGL::Size<uint> newSize(
        (uint)std::max((int)fMinWindowSize.getWidth(), bottomRight.getX()),
        (uint)std::max((int)fMinWindowSize.getHeight(), bottomRight.getY()));

    if (RequestToResizeCallback)
        RequestToResizeCallback(newSize);

    return true;
}

void ResizeHandle::onNanoDisplay()
{
    const ColorPalette &cp = fPalette;

    const int w = getWidth();
    const int h = getHeight();
    const Rect bounds(0, 0, w, h);

    const float intervalPx = 4.0;
    for (float d = intervalPx; d < w && d < h; d += intervalPx) {
        beginPath();
        moveTo(w - d, h);
        lineTo(w, h - d);
        strokeColor(Colors::fromRGBA8(cp[Colors::resize_handle]));
        stroke();
    }
}
