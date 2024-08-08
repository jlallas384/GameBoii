#pragma once

#include <cstdint>

using Color = uint16_t;

class LCD {
public:
    constexpr static uint8_t height = 144, width = 160;
    constexpr static Color white = 0;
    virtual ~LCD() = default;
    virtual void refresh() = 0;
    virtual void setPixel(uint8_t i, uint8_t j, Color color) = 0;
};