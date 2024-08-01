#pragma once

#include <cstdint>


class LCD {
public:
    constexpr static uint8_t height = 144, width = 160, white = 0;
    virtual ~LCD() = default;
    virtual void setPixel(uint8_t i, uint8_t j, uint8_t color) = 0;
};