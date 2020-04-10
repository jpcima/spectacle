#include "TextLabel.h"
#include "Window.hpp"
#include "Cairo.hpp"

TextLabel::TextLabel(Widget *group, FontEngine &fontEngine)
    : Widget(group),
      fFontEngine(fontEngine)
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

void TextLabel::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    FontEngine &fe = fFontEngine;

    Rect box{0, 0, (int)getWidth(), (int)getHeight()};
    fe.drawInBox(cr, fText.c_str(), fFont, box, fAlign);
}
