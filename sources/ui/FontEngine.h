#pragma once
#include "Fontstash.h"
#include "Color.h"
#include "Geometry.h"
#include "Cairo++.h"
#include <cstdint>

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
    FontEngine();
    FontEngine(unsigned width, unsigned height);
    ~FontEngine();

    bool addFont(const char *name, const uint8_t *data, unsigned size);

    void draw(cairo_t *cr, const char *text, const Font &font, double x, double y);
    void drawInBox(cairo_t *cr, const char *text, const Font &font, const Rect &box, int align);
    void drawInBox(cairo_t *cr, const char *text, const Font &font, const RectF &box, int align);

private:
    static int renderCreate(void *uptr, int width, int height);
    static int renderResize(void *uptr, int width, int height);
    static void renderUpdate(void *uptr, int *rect, const unsigned char *data);
    static void renderDraw(void *uptr, const FONSquad *quads, const unsigned int *colors, int nquads);
    static void renderDelete(void *uptr);

private:
    FONScontext_u fContext;
    cairo_surface_u fAtlas;
    cairo_t *fDrawingContext = nullptr;
};

///
struct Font {
    std::string name = "default";
    float size = 12.0;
    ColorRGBA8 color = {0x00, 0x00, 0x00, 0xff};
    float spacing = 0.0;
    float blur = 0;
};

inline bool operator==(const Font &a, const Font &b)
{
    return a.name == b.name && a.size == b.size && a.color == b.color &&
        a.spacing == b.spacing && a.blur == b.blur;
}

inline bool operator!=(const Font &a, const Font &b)
{
    return !operator==(a, b);
}
