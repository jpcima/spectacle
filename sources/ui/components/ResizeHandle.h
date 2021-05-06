#pragma once
#include "NanoVG.hpp"
class ColorPalette;

class ResizeHandle : public NanoWidget {
public:
    ResizeHandle(Widget *group, const ColorPalette &palette);

    void onNanoDisplay() override;

private:
    const ColorPalette &fPalette;
};
