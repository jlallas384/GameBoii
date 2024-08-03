#pragma once

#include <span>
#include <cstdint>

class Tile {
public:
    Tile() {}
    Tile(std::span<const uint8_t> mem);
    uint8_t at(uint8_t i, uint8_t j) const;
private:
    std::span<const uint8_t> mem;
};