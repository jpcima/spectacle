#include "ColorPalette.h"
#include "Config.h"
#include <unordered_set>
#include <cstdio>
#include <cstring>

ColorPalette::ColorPalette()
    : colors_{new ColorRGBA8[Colors::Count]{}}
{
}

ColorPalette ColorPalette::create_default()
{
    std::unique_ptr<CSimpleIniA> ini = create_configuration();
    ColorPalette::save_defaults(*ini, "color", true);

    ColorPalette pal;
    pal.load(*ini, "color");

    return pal;
}

ColorPalette &ColorPalette::get_current()
{
    static ColorPalette palette = create_default();

    return palette;
}

///
static int hex_digit_from_char(char c)
{
    return (c >= '0' && c <= '9') ? (c - '0') :
        (c >= 'a' && c <= 'z') ? (c - 'a' + 10) :
        (c >= 'A' && c <= 'Z') ? (c - 'A' + 10) : -1;
}

static bool color_from_hex(const char *hex, ColorRGBA8 &color)
{
    if (hex[0] == '\0' || hex[0] != '#')
        return false;

    ++hex;
    size_t length = std::strlen(hex);

    uint32_t rgba = 0;
    if (length == 6 || length == 8) {
        for (size_t i = 0; i < length; ++i) {
            int d = hex_digit_from_char(hex[i]);
            if (d == -1)
                return false;
            rgba = (rgba << 4) | d;
        }
        if (length == 6)
            rgba = (rgba << 8) | 0xff;
    }

    color.r = rgba >> 24;
    color.g = (rgba >> 16) & 0xff;
    color.b = (rgba >> 8) & 0xff;
    color.a = rgba & 0xff;

    return true;
}

static std::string hex_from_color(ColorRGBA8 color)
{
    char rgb_string[16];
    if (color.a == 0xff)
        sprintf(rgb_string, "#%02x%02x%02x", color.r, color.g, color.b);
    else
        sprintf(rgb_string, "#%02x%02x%02x%02x", color.r, color.g, color.b, color.a);
    return rgb_string;
}

///
bool ColorPalette::load(const CSimpleIniA &ini, const char *section)
{
    const CSimpleIniA::TKeyVal *dict = ini.GetSection(section);

    if (!dict)
        return false;

    bool is_color_set[Colors::Count] = {};
    size_t num_colors_set = 0;

    CSimpleIniA::TKeyVal::const_iterator it = dict->begin(), end = dict->end();
    for (; it != end; ++it) {
        const char *key = it->first.pItem;
        const char *value = it->second;

        unsigned index = ~0u;
        for (unsigned i = 0; i < Colors::Count && index == ~0u; ++i) {
            if (std::strcmp(key, Colors::Name[i]) == 0)
                index = i;
        }

        if (index == ~0u)
            continue;

        bool found = false;
        if (color_from_hex(value, colors_[index]))
            found = true;
        else {
            // try to look it up from another key
            std::unordered_set<std::string> met;
            const char *key = value;
            while (!found) {
                auto it = dict->find(key);
                if (it == dict->end() || !met.insert(key).second)
                    break;
                key = it->second;
                found = color_from_hex(key, colors_[index]);
            }
        }

        if (!found) {
            fprintf(stderr, "Colors: cannot interpret the color value \"%s\" for \"%s\".\n", value, key);
            continue;
        }

        if (!is_color_set[index]) {
            is_color_set[index] = true;
            ++num_colors_set;
        }
    }

    if (num_colors_set != Colors::Count)
        fprintf(stderr, "Colors: the color palette is incomplete.\n");

    return true;
}

void ColorPalette::save_defaults(CSimpleIniA &ini, const char *section, bool overwrite)
{
    auto fill_color = [&ini, section, overwrite](int color, const char *value, const char *comment) {
        ini.SetValue(section, Colors::Name[color], value, comment, overwrite);
    };

    fill_color(Colors::text_normal, "#ffffff", nullptr);
    fill_color(Colors::text_active, "#ffaa00", nullptr);

    auto hueFromChannel = [](uint32_t channel) {
        double h = 0.5 + channel / 3.0;
        h -= (long)h;
        return h;
    };

    for (uint32_t channel = 0; channel < DISTRHO_PLUGIN_NUM_INPUTS; ++channel) {
        const ColorRGBA8 colorText = Colors::toRGBA8(
            DGL::Color::fromHSL(hueFromChannel(channel), 0.8, 0.5));
        fill_color(Colors::text_channel1 + channel, hex_from_color(colorText).c_str(), nullptr);
    }

    fill_color(Colors::spectrum_background, "#202033", nullptr);

    fill_color(Colors::spectrum_grid_text, "#4c4c4c80", nullptr);
    fill_color(Colors::spectrum_grid_lines, "spectrum-grid-text", nullptr);

    for (uint32_t channel = 0; channel < DISTRHO_PLUGIN_NUM_INPUTS; ++channel) {
        const ColorRGBA8 colorStroke = Colors::toRGBA8(
            DGL::Color::fromHSL(hueFromChannel(channel), 0.8, 0.3));
        const ColorRGBA8 colorFill =
            { colorStroke.r, colorStroke.g, colorStroke.b, 0x10 };
        fill_color(
            Colors::spectrum_line_channel1 + channel,
            hex_from_color(colorStroke).c_str(), nullptr);
        fill_color(
            Colors::spectrum_fill_channel1 + channel,
            hex_from_color(colorFill).c_str(), nullptr);
    }

    fill_color(Colors::spectrum_select_line, "#ffaa0040", nullptr);

    fill_color(Colors::slider_back, "#262626", nullptr);
    fill_color(Colors::slider_fill, "#404040", nullptr);

    fill_color(Colors::spin_box_back, "slider-back", nullptr);
    fill_color(Colors::spin_box_fill, "slider-fill", nullptr);

    fill_color(Colors::floating_window_back, "#00000040", nullptr);
    fill_color(Colors::tool_bar_back, "floating-window-back", nullptr);
}
