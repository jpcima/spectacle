#pragma once
#include <SimpleIni.h>
#include "ui/Color.h"
#include <memory>
#include <cassert>

#if !defined(maybe_unused)
#   if defined(__GNUC__)
#       define maybe_unused __attribute__((unused))
#   else
#       define maybe_unused
#   endif
#endif

struct ColorPalette
{
    ColorPalette();

    static ColorPalette create_default();
    static ColorPalette &get_current();

    bool load(const CSimpleIniA &ini, const char *section);
    static void save_defaults(CSimpleIniA &ini, const char *section, bool overwrite);

    ColorRGBA8 &operator[](size_t index) noexcept;
    const ColorRGBA8 &operator[](size_t index) const noexcept;

    static constexpr ColorRGBA8 transparent() noexcept
        { return ColorRGBA8{0, 0, 0, 0}; }

private:
    std::unique_ptr<ColorRGBA8[]> colors_;
};

namespace Colors {
    enum {
        background,
        info_box_background,
        text_browser_foreground,
        metadata_label,
        metadata_value,
        text_min_brightness,
        text_low_brightness,
        text_high_brightness,
        piano_white_key,
        piano_white_shadow,
        piano_black_key,
        piano_pressed_key,
        digit_on,
        digit_off,
        box_frame,
        box_background,
        box_title,
        box_foreground,
        box_foreground_secondary,
        box_active_item_background,
        box_active_item_foreground,
    };

    maybe_unused static const char *Name[] = {
        "background",
        "info-box-background",
        "text-browser-foreground",
        "metadata-label",
        "metadata-value",
        "text-min-brightness",
        "text-low-brightness",
        "text-high-brightness",
        "piano-white-key",
        "piano-white-shadow",
        "piano-black-key",
        "piano-pressed-key",
        "digit-on",
        "digit-off",
        "box-frame",
        "box-background",
        "box-title",
        "box-foreground",
        "box-foreground-secondary",
        "box-active-item-background",
        "box-active-item-foreground",
    };

    maybe_unused static constexpr size_t Count =
        sizeof(Name) / sizeof(Name[0]);
}

inline ColorRGBA8 &ColorPalette::operator[](size_t index) noexcept
{
    assert(index < Colors::Count);
    return colors_[index];
}

inline const ColorRGBA8 &ColorPalette::operator[](size_t index) const noexcept
{
    assert(index < Colors::Count);
    return colors_[index];
}
