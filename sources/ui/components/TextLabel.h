#pragma once
#include "NanoVG.hpp"
#include "ui/FontEngine.h"
class ColorPalette;

class TextLabel : public NanoSubWidget {
public:
    TextLabel(Widget *group, const ColorPalette &palette);

    const Font &font() const { return fFont; };
    void setFont(const Font &font);

    const std::string &text() const { return fText; }
    void setText(std::string text);

    int alignment() const { return fAlign; }
    void setAlignment(int align);

protected:
    void onNanoDisplay() override;

private:
    const ColorPalette &fPalette;
    Font fFont;
    std::string fText;
    int fAlign = kAlignLeft|kAlignTop|kAlignInside;
};
