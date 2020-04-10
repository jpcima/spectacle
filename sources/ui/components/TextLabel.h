#pragma once
#include "Widget.hpp"
#include "ui/FontEngine.h"

class TextLabel : public Widget {
public:
    TextLabel(Widget *group, FontEngine &fontEngine);

    const Font &font() const { return fFont; };
    void setFont(const Font &font);

    const std::string &text() const { return fText; }
    void setText(std::string text);

    int alignment() const { return fAlign; }
    void setAlignment(int align);

protected:
    void onDisplay() override;

private:
    FontEngine &fFontEngine;
    Font fFont;
    std::string fText;
    int fAlign = kAlignLeft|kAlignTop|kAlignInside;
};
