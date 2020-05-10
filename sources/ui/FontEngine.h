#pragma once
#include "Color.h"
#include "Geometry.h"
#include "NanoVG.hpp"
#include <vector>
#include <cstdint>
class ColorPalette;

struct Font;

///
enum FontAlign {
    kAlignCenter = 0,
    kAlignTop = 1,
    kAlignBottom = 2,
    kAlignLeft = 4,
    kAlignRight = 8,
    kAlignInside = 16,
    kAlignTopLeft = kAlignTop|kAlignLeft,
    kAlignTopRight = kAlignTop|kAlignRight,
    kAlignBottomLeft = kAlignBottom|kAlignLeft,
    kAlignBottomRight = kAlignBottom|kAlignRight,
};

///
class FontEngine {
public:
    explicit FontEngine(NanoVG &vg, const ColorPalette &palette);

    void draw(const char *text, const Font &font, double x, double y);
    void drawInBox(const char *text, const Font &font, const Rect &box, int align);
    void drawInBox(const char *text, const Font &font, const RectF &box, int align);

private:
    int lazyFont(const char *name);

private:
    NanoVG &fVg;
    const ColorPalette &fColorPalette;
};

///
struct Font {
    std::string name = "default";
    float size = 12.0;
    int colorRef = -1;
    ColorRGBA8 color = {0x00, 0x00, 0x00, 0xff};
};

inline bool operator==(const Font &a, const Font &b)
{
    return
        a.name == b.name &&
        a.size == b.size &&
        a.colorRef == b.colorRef &&
        a.color == b.color;
}

inline bool operator!=(const Font &a, const Font &b)
{
    return !operator==(a, b);
}
