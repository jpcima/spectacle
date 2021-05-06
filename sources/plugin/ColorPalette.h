#pragma once
#include <SimpleIni.h>
#include "ui/Color.h"
#include "DistrhoPluginInfo.h"
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
        text_normal,
        text_active,
        text_channel1,
        text_channelN = text_channel1 + DISTRHO_PLUGIN_NUM_INPUTS - 1,
        spectrum_background,
        spectrum_grid_text,
        spectrum_grid_lines,
        spectrum_minor_grid_lines,
        spectrum_line_channel1,
        spectrum_line_channelN = spectrum_line_channel1 + DISTRHO_PLUGIN_NUM_INPUTS - 1,
        spectrum_fill_channel1,
        spectrum_fill_channelN = spectrum_fill_channel1 + DISTRHO_PLUGIN_NUM_INPUTS - 1,
        spectrum_select_line,
        slider_back,
        slider_fill,
        spin_box_back,
        spin_box_fill,
        floating_window_back,
        tool_bar_back,
        selection_rectangle,
        resize_handle,
    };

    maybe_unused static const char *Name[] = {
        "text-normal",
        "text-active",
        "text-channel1",
        "text-channel2",
        "spectrum-background",
        "spectrum-grid-text",
        "spectrum-grid-lines",
        "spectrum-minor-grid-lines",
        "spectrum-line-channel1",
        "spectrum-line-channel2",
        "spectrum-fill-channel1",
        "spectrum-fill-channel2",
        "spectrum-select-line",
        "slider-back",
        "slider-fill",
        "spin-box-back",
        "spin-box-fill",
        "floating-window-back",
        "tool-bar-back",
        "selection-rectangle",
        "resize-handle",
        #if DISTRHO_PLUGIN_NUM_INPUTS != 2
        #  error currently the name list assumes channels = 2
        #endif
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
