#include "ppu.h"
#include <algorithm>
#include "irq_handler.h"
#include "tile.h"
#include "address_bus.h"
#include "utils.h"
#include "object_layer.h"

PPU::PPU(AddressBus& addrBus, std::unique_ptr<LCD> lcd, IRQHandler& irqHandler) : lcd(std::move(lcd)), irqHandler(irqHandler) {
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
    return getBit(lcdc, 3) ? getTileAtTileMap2(i, j) : getTileAtTileMap1(i, j);
}

uint8_t PPU::getBackgroundColorIdAt(uint8_t i, uint8_t j) const {
    i = (i + scy) % 256, j = (j + scx) % 256;
    Tile t = getBackgroundTileAt(i / 32, j / 32);
    return t.at(i % 32, j % 32);
}

bool PPU::isIntersectAtWindow(uint8_t i, uint8_t j) const {
    return wy <= i && wx - 7 <= j && j < wx - 7 + 256;
}

uint8_t PPU::getWindowColorIdAt(uint8_t i, uint8_t j) const {
    i -= wy;
    j -= wx - 7;
    Tile t = getWindowTileAt(i / 32, j / 32);
    return t.at(i % 32, j % 32);
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
    std::stable_sort(ret.begin(), ret.end());
    return ret;
}

uint8_t PPU::getPaletteColor(uint8_t palette, uint8_t id) {
    return (palette >> (id * 2)) & 0x3;
}

void PPU::doSingleScanline() {
    auto objects = getObjectsToRender();
    for (int x = 0; x < LCD::width; x++) {
        uint8_t bgId = 4, bgColor = LCD::white;
        if (getBit(lcdc, 0)) {
            uint8_t id = getBackgroundColorIdAt(ly, x);
            uint8_t color = getPaletteColor(bgp, id);
            bgId = id, bgColor = color;
            if (getBit(lcdc, 5) && isIntersectAtWindow(ly, x)) {
                uint8_t id = getWindowColorIdAt(ly, x);
                uint8_t color = getPaletteColor(bgp, id);
                bgId = id, bgColor = color;
            }
        }
        lcd->setPixel(ly, x, bgColor);
        if (!getBit(lcdc, 1)) continue;
        for (const auto& obj : objects) {
            if (obj.isIntersectAtPoint(ly, x)) {
                uint8_t id = obj.getColorIdAt(ly, x);
                if (id == 0) continue;
                uint8_t palette = obj.getPalette(obp0, obp1);
                uint8_t color = getPaletteColor(palette, id);
                if (obj.isDraw(bgId)) {
                    lcd->setPixel(ly, x, color);
                }
                break;
            }
        }
    }
    ly++;
}

