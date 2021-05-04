#include "SelectionRectangle.h"
#include "plugin/ColorPalette.h"
#include "Window.hpp"

SelectionRectangle::SelectionRectangle(Widget *group, ColorPalette &palette)
    : NanoWidget(group),
      fPalette(palette)
{
}

void SelectionRectangle::onNanoDisplay()
{
    ColorPalette &cp = fPalette;

    const int w = getWidth();
    const int h = getHeight();

    const int dashSize = 4;
    const int numDashesX = w / (2 * dashSize - 1);
    const int numDashesY = h / (2 * dashSize - 1);

    beginPath();
    for (int dashX = 0; dashX < numDashesX; ++dashX) {
        int x = dashX * (2 * dashSize);
        moveTo(x, 0.5);
        lineTo(x + dashSize, 0.5);
        moveTo(x, (int)h - 0.5);
        lineTo(x + dashSize, (int)h - 0.5);
    }
    for (int dashY = 0; dashY < numDashesY; ++dashY) {
        int y = dashY * (2 * dashSize);
        moveTo(0.5, y);
        lineTo(0.5, y + dashSize);
        moveTo((int)w - 0.5, y);
        lineTo((int)w - 0.5, y + dashSize);
    }
    strokeColor(Colors::fromRGBA8(cp[Colors::selection_rectangle]));
    strokeWidth(1.0);
    stroke();
}
