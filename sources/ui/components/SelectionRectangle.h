#pragma once
#include "NanoVG.hpp"
class ColorPalette;

class SelectionRectangle : public NanoWidget {
public:
    explicit SelectionRectangle(Widget *group, ColorPalette &palette);
    void onNanoDisplay() override;

private:
    ColorPalette &fPalette;
};
