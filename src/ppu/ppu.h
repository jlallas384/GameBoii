#pragma once
#include <array>
#include <cstdint>
#include <vector>

class AddressBus;
class Tile;

struct ObjectData;
class ObjectLayer;

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
    ObjectLayer createObject(uint8_t index) const;
    std::vector<ObjectLayer> getObjectsToRender() const;

    std::array<uint8_t, 1 << 13> vram{};
    std::array<uint8_t, 160> oam{};
    uint8_t lcdc = 0, ly = 0, scy, scx;
};