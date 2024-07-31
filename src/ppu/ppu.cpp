#include "ppu.h"
#include <algorithm>
#include "tile.h"
#include "address_bus.h"
#include "utils.h"
#include "object_layer.h"

PPU::PPU(AddressBus& addrBus) {
    for (int i = 0; i < (1 << 13); i++) {
        addrBus.setReader(0x8000 + i, vram[i]);
        addrBus.setWriter(0x8000 + i, vram[i]);
    }
    for (int i = 0; i < 160; i++) {
        addrBus.setReader(0xfe00 + i, oam[i]);
        addrBus.setWriter(0xfe00 + i, oam[i]);
    }
    addrBus.setWriter(0xff46, [&](uint8_t byte) {
        for (int i = 0; i < 160; i++) {
            oam[i] = addrBus.read((byte << 2) | i);
        }
    });
    addrBus.setReader(0xff42, scy);
    addrBus.setWriter(0xff42, scy);
    addrBus.setReader(0xff43, scx);
    addrBus.setWriter(0xff43, scx);
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
    i = (i + scy) % 256, j = (j + scx) % 256;
    return getBit(lcdc, 3) ? getTileAtTileMap2(i, j) : getTileAtTileMap1(i, j);
}

ObjectLayer PPU::createObject(uint8_t index) const {
    index *= 4;
    ObjectData data = { oam[index], oam[index + 1], oam[index + 2], oam[index + 3] };
    if (!getBit(lcdc, 2)) {
        return ObjectLayer(getObjectTile(data.tileIndex), data);
    } else {
        Tile t1 = getObjectTile(data.tileIndex & 0xfe);
        Tile t2 = getObjectTile(data.tileIndex | 0x1);
        return ObjectLayer(t1, t2, data);
    }
}

std::vector<ObjectLayer> PPU::getObjectsToRender() const {
    std::vector<ObjectLayer> ret;
    for (int i = 0; i < 40 && ret.size() < 10; i++) {
        auto object = createObject(i);
        if (object.isAtScanline(ly)) {
            ret.push_back(object);
        }
    }
    std::reverse(ret.begin(), ret.end());
    std::stable_sort(ret.begin(), ret.end());
    return ret;
}
    
