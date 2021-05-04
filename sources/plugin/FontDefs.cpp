#include "FontDefs.h"

static constexpr uint8_t fontRegular[] = {
    #include "fonts/liberation/LiberationSans-Regular.ttf.h"
};
static constexpr uint8_t fontAwesome[] = {
    #include "fonts/fontawesome/Font-Awesome-5-Free-Solid-900.otf.h"
};

const FontDef fontDefinitions[] = {
    {"regular", fontRegular, sizeof(fontRegular)},
    {"awesome", fontAwesome, sizeof(fontAwesome)},
    {},
};
