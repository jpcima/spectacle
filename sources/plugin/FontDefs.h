#pragma once
#include <cstdint>

struct FontDef {
    const char *name;
    const uint8_t *data;
    uint32_t size;
};

extern const FontDef fontDefinitions[];
