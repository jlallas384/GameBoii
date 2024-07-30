#include "ppu.h"
#include "tile.h"
#include "address_bus.h"
#include "utils.h"

PPU::PPU(AddressBus& addrBus) {
    for (int i = 0; i < (1 << 13); i++) {
        addrBus.setReader(0x8000 + i, vram[i]);
        addrBus.setWriter(0x8000 + i, vram[i]);
    }
}

Tile PPU::getObjectTile(uint8_t index) const {
    return Tile(std::span(vram.begin() + index * 16, 16));
}

Tile PPU::getNonObjectTile(uint8_t index) const {
    return getBit(lcdc, 4) ? Tile(std::span(vram.begin() + index * 16, 16))
        : Tile(std::span(vram.begin() + 0x1000 + static_cast<int8_t>(index) * 16, 16));
}

Tile PPU::getTileAtTileMap1(uint8_t i, uint8_t j) const {
    auto index = 0x9800 + i * 32 + j;
    return getNonObjectTile(vram[index]);
}

Tile PPU::getTileAtTileMap2(uint8_t i, uint8_t j) const {
    auto index = 0x9c00 + i * 32 + j;
    return getNonObjectTile(vram[index]);
}

Tile PPU::getWindowTileAt(uint8_t i, uint8_t j) const {
    return getBit(lcdc, 7) ? getTileAtTileMap2(i, j) : getTileAtTileMap1(i, j);
}

Tile PPU::getBackgroundTileAt(uint8_t i, uint8_t j) const {
    return getBit(lcdc, 3) ? getTileAtTileMap2(i, j) : getTileAtTileMap1(i, j);
}
