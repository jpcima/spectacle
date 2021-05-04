#include "FontEngine.h"
#include "plugin/ColorPalette.h"
#include "plugin/FontDefs.h"
#include <stdexcept>
#include <cstring>

FontEngine::FontEngine(NanoVG &vg, const ColorPalette &palette)
    : fVg(vg), fColorPalette(palette)
{
}

void FontEngine::draw(const char *text, const Font &font, double x, double y)
{
    NanoVG &vg = fVg;

    const char *name = font.name.empty() ? "default" : font.name.c_str();
    int id = lazyFont(name);

    if (id == -1)
        return;

    ColorRGBA8 color = (font.colorRef == -1) ? font.color : fColorPalette[font.colorRef];

    vg.beginPath();
    vg.fontFaceId(id);
    vg.fontSize(font.size);
    vg.fillColor(Colors::fromRGBA8(color));
    vg.textAlign(0);
    vg.text(x, y, text, nullptr);
    vg.closePath();
}

void FontEngine::drawInBox(const char *text, const Font &font, const Rect &box, int align)
{
    return drawInBox(text, font, box.to<double>(), align);
}

void FontEngine::drawInBox(const char *text, const Font &font, const RectF &box, int align)
{
    NanoVG &vg = fVg;

    const char *name = font.name.empty() ? "default" : font.name.c_str();
    int id = lazyFont(name);

    if (id == -1)
        return;

    ColorRGBA8 color = (font.colorRef == -1) ? font.color : fColorPalette[font.colorRef];

    double x = box.x;
    double y = box.y;
    double w = box.w;
    double h = box.h;

    int fonsalign;

    switch (align & (kAlignLeft|kAlignRight)) {
    default:
        fonsalign = NanoVG::ALIGN_CENTER;
        x += 0.5 * w;
        break;
    case kAlignLeft:
        fonsalign = (align & kAlignInside) ? NanoVG::ALIGN_LEFT : NanoVG::ALIGN_RIGHT;
        break;
    case kAlignRight:
        fonsalign = (align & kAlignInside) ? NanoVG::ALIGN_RIGHT : NanoVG::ALIGN_LEFT;
        x += w;
        break;
    }

    switch (align & (kAlignTop|kAlignBottom)) {
    default:
        fonsalign |= NanoVG::ALIGN_MIDDLE;
        y += 0.5 * h;
        break;
    case kAlignTop:
        fonsalign |= (align & kAlignInside) ? NanoVG::ALIGN_TOP : NanoVG::ALIGN_BOTTOM;
        break;
    case kAlignBottom:
        fonsalign |= (align & kAlignInside) ? NanoVG::ALIGN_BOTTOM : NanoVG::ALIGN_TOP;
        y += h;
        break;
    }

    vg.beginPath();
    vg.fontFaceId(id);
    vg.fontSize(font.size);
    vg.fillColor(Colors::fromRGBA8(color));
    vg.textAlign(fonsalign);
    vg.text(x, y, text, nullptr);
    vg.closePath();
}

int FontEngine::lazyFont(const char *name)
{
    NanoVG &vg = fVg;
    int id = vg.findFont(name);

    if (id == -1) {
        const FontDef *fontdef = nullptr;

        for (const FontDef *p = fontDefinitions; p->name && !fontdef; ++p) {
            if (!std::strcmp(p->name, name))
                fontdef = p;
        }

        if (fontdef)
            id = fVg.createFontFromMemory(name, fontdef->data, fontdef->size, false);
    }

    return id;
}
