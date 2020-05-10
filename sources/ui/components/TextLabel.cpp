#include "TextLabel.h"
#include "Window.hpp"
#include "plugin/ColorPalette.h"

TextLabel::TextLabel(Widget *group, const ColorPalette &palette)
    : NanoWidget(group),
      fPalette(palette)
{
}

void TextLabel::setFont(const Font &font)
{
    if (fFont == font)
        return;

    fFont = font;
    repaint();
}

void TextLabel::setText(std::string text)
{
    if (fText == text)
        return;

    fText = std::move(text);
    repaint();
}

void TextLabel::setAlignment(int align)
{
    if (fAlign == align)
        return;

    fAlign = align;
    repaint();
}

void TextLabel::onNanoDisplay()
{
    const ColorPalette &cp = fPalette;
    FontEngine fe(*this, cp);

    Rect box{0, 0, (int)getWidth(), (int)getHeight()};
    fe.drawInBox(fText.c_str(), fFont, box, fAlign);
}
