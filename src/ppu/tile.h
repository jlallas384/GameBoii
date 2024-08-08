#pragma once

#include <span>
#include <cstdint>

class Tile {
public:
    Tile(std::span<const uint8_t> mem);
    uint8_t at(uint8_t i, uint8_t j) const;
private:
    std::span<const uint8_t> mem;
};

class BackgroundTile : public Tile {
public:
    BackgroundTile(std::span<const uint8_t> mem, uint8_t attribute);
    uint8_t at(uint8_t i, uint8_t j) const;
    uint8_t getPalette();
private:
    bool priority, flipY, flipX;
    uint8_t palette;
};