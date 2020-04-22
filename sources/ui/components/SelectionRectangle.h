#pragma once
#include "Widget.hpp"
class ColorPalette;

class SelectionRectangle : public Widget {
public:
    explicit SelectionRectangle(Widget *group, ColorPalette &palette);
    void onDisplay() override;

private:
    ColorPalette &fPalette;
};
