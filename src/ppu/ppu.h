#pragma once
#include <array>
#include <cstdint>

class AddressBus;
class Tile;

class PPU {
public:
    PPU(AddressBus& addrBus);
private:
    Tile getObjectTile(uint8_t index) const;
    Tile getNonObjectTile(uint8_t index) const;
    Tile getTileAtTileMap1(uint8_t i, uint8_t j) const;
    Tile getTileAtTileMap2(uint8_t i, uint8_t j) const;
    Tile getWindowTileAt(uint8_t i, uint8_t j) const;
    Tile getBackgroundTileAt(uint8_t i, uint8_t j) const;
    std::array<uint8_t, 1 << 13> vram{};
    uint8_t lcdc = 0;
};