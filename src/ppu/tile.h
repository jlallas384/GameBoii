#pragma once

#include <array>
#include <span>
#include <cstdint>

class Tile {
public:
    Tile() {}
    Tile(std::span<const uint8_t> mem);
    uint8_t at(uint8_t i, uint8_t j) const;
private:
    std::array<uint16_t, 8> arr{};
};