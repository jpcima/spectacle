#pragma once

struct ColorRGBA8 {
    uint8_t r, g, b, a;
};

inline bool operator==(ColorRGBA8 a, ColorRGBA8 b) noexcept
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

inline bool operator!=(ColorRGBA8 a, ColorRGBA8 b) noexcept
{
    return !operator==(a, b);
}

///
#include "Color.hpp"

namespace Colors {
    inline ColorRGBA8 toRGBA8(Color c)
    {
        ColorRGBA8 c8;
        c8.r = std::max(0, std::min(0xff, int(0xff * c.red)));
        c8.g = std::max(0, std::min(0xff, int(0xff * c.green)));
        c8.b = std::max(0, std::min(0xff, int(0xff * c.blue)));
        c8.a = 0xff;
        return c8;
    }

    inline Color fromHSV(float h, float sv, float v)
    {
        float l = v * (1.0f - 0.5f * sv);
        float sl;
        if (l <= 0 || l >= 1)
            sl = 0;
        else
            sl = (v - l) / std::min(l, 1 - l);
        return Color::fromHSL(h, sl, l);
    }
}
