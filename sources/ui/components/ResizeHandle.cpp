#include "ResizeHandle.h"
#include "ui/Geometry.h"
#include "ui/NanoVGHelpers.h"
#include "plugin/ColorPalette.h"

ResizeHandle::ResizeHandle(Widget *group, const ColorPalette &palette)
    : NanoWidget(group),
      fPalette(palette)
{
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


    // TODO
    
    
}
