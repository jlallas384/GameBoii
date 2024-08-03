#include "ppu.h"
#include <algorithm>
#include "irq_handler.h"
#include "tile.h"
#include "address_bus.h"
#include "utils.h"
#include "object_layer.h"


PPU::PPU(AddressBus& addrBus, std::unique_ptr<LCD> lcd, IRQHandler& irqHandler) : lcd(std::move(lcd)), irqHandler(irqHandler) {
    for (int i = 0; i < (1 << 13); i++) {
        addrBus.setReader(0x8000 + i, [&, i]() {
            return currentMode != kDrawing ? vram[i] : 0xff;
        });
        addrBus.setWriter(0x8000 + i, [&, i](uint8_t byte) {
            if (currentMode != kDrawing) {
                vram[i] = byte;
            }
        });
    }
    for (int i = 0; i < 160; i++) {
        addrBus.setReader(0xfe00 + i, [&, i]() {
            return currentMode != kOAMScan && currentMode != kDrawing ? oam[i] : 0xff;
        });
        addrBus.setWriter(0xfe00 + i, [&, i](uint8_t byte) {
            if (currentMode != kOAMScan && currentMode != kDrawing) {
                oam[i] = byte;
            }
        });
    }
    addrBus.setWriter(0xff46, [&](uint8_t byte) {
        for (int i = 0; i < 160; i++) {
            oam[i] = addrBus.read((byte << 8) | i);
        }
    });
    addrBus.setReader(0xff40, lcdc);
    addrBus.setWriter(0xff40, [&](uint8_t byte) {
        bool enabled = getBit(byte, 7);
        if (currentMode != kDisabled && !enabled) {
            nextMode = kDisabled;
        } else if (currentMode == kDisabled && enabled) {
            nextMode = kOAMScan;
            ly = 0;
        }
        lcdc = byte;
    });

    addrBus.setReader(0xff41, stat);
    addrBus.setWriter(0xff41, [&](uint8_t byte) {
        stat = byte & ~0x3;
    });

    addrBus.setReader(0xff42, scy);
    addrBus.setWriter(0xff42, scy);
    addrBus.setReader(0xff43, scx);
    addrBus.setWriter(0xff43, scx);

    addrBus.setReader(0xff44, ly);
    addrBus.setWriter(0xff44, ly);

    addrBus.setReader(0xff45, lyc);
    addrBus.setWriter(0xff45, [&](uint8_t byte) {
        lyc = byte;
        doLYCompare();
    });

    addrBus.setReader(0xff4a, wy);
    addrBus.setWriter(0xff4a, wy);
    addrBus.setReader(0xff4b, wx);
    addrBus.setWriter(0xff4b, wx);

    addrBus.setReader(0xff47, bgp);
    addrBus.setWriter(0xff47, bgp);

    addrBus.setReader(0xff48, obp0);
    addrBus.setWriter(0xff48, obp0);
    addrBus.setReader(0xff49, obp1);
    addrBus.setWriter(0xff49, obp1);
}

void PPU::tick() {
    if (currentMode != nextMode) {
        currentMode = nextMode;
        state.x = 0;
        tickCount = 0;
        stat = (stat & ~0x3) | (currentMode & 0x3);
    }
    tickCount++;

    switch (currentMode) {
        case kOAMScan:
            if (tickCount == 1) {
                if (getBit(stat, 5)) {
                    irqHandler.request(IRQHandler::kStat);
                }
                state.scanlineObjects = getObjectsToRender();
                doLYCompare();
            }
            if (tickCount == 80) {
                nextMode = kDrawing;
            }
            break;
        case kDrawing:
            if (state.x < LCD::width) {
                doSingleDotDrawing();
            }
            if (tickCount == 172) {
                nextMode = kHBlank;
            }
            break;
        case kHBlank:
            if (tickCount == 1 && getBit(stat, 3)) {
                irqHandler.request(IRQHandler::kStat);
            }
            if (tickCount == 204) {
                ly++;
                if (ly < LCD::height) {
                    nextMode = kOAMScan;
                } else {
                    nextMode = kVBlank;
                }
            }
            break;
        case kVBlank:
            if (tickCount == 1) {
                doLYCompare();
                if (getBit(stat, 1)) {
                    irqHandler.request(IRQHandler::kStat);
                }
                irqHandler.request(IRQHandler::kVBlank);
            }
            if (tickCount == 456) {
                ly++;
                tickCount = 0;
            }
            if (ly == 154) {
                nextMode = kOAMScan;
                lcd->refresh();
                ly = 0;
            }
            break;
        case kDisabled:
            break;
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
    auto index = 0x1800 + i * 32 + j;
    return getNonObjectTile(vram[index]);
}

Tile PPU::getTileAtTileMap2(uint8_t i, uint8_t j) const {
    auto index = 0x1c00 + i * 32 + j;
    return getNonObjectTile(vram[index]);
}

Tile PPU::getWindowTileAt(uint8_t i, uint8_t j) const {
    return getBit(lcdc, 6) ? getTileAtTileMap2(i, j) : getTileAtTileMap1(i, j);
}

Tile PPU::getBackgroundTileAt(uint8_t i, uint8_t j) const {
    return getBit(lcdc, 3) ? getTileAtTileMap2(i, j) : getTileAtTileMap1(i, j);
}

uint8_t PPU::getBackgroundColorIdAt(uint8_t i, uint8_t j) const {
    i = (i + scy) % 256, j = (j + scx) % 256;
    Tile t = getBackgroundTileAt(i / 8, j / 8);
    return t.at(i % 8, j % 8);
}

bool PPU::isIntersectAtWindow(uint8_t i, uint8_t j) const {
    return wy <= i && wx - 7 <= j && j < wx - 7 + 256;
}

uint8_t PPU::getWindowColorIdAt(uint8_t i, uint8_t j) const {
    i -= wy;
    j -= wx - 7;
    Tile t = getWindowTileAt(i / 8, j / 8);
    return t.at(i % 8, j % 8);
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

uint8_t PPU::getPaletteColor(uint8_t palette, uint8_t id) const {
    return (palette >> (id * 2)) & 0x3;
}

void PPU::doLYCompare() {
    bool res = ly == lyc;
    stat = setBit(stat, 2, res);
    if (res && getBit(stat, 6)) {
        irqHandler.request(IRQHandler::kStat);
    }
}

void PPU::doSingleDotDrawing() {
    uint8_t bgId = 4, bgColor = LCD::white;
    if (getBit(lcdc, 0)) {
        uint8_t id = getBackgroundColorIdAt(ly, state.x);
        uint8_t color = getPaletteColor(bgp, id);
        bgId = id, bgColor = color;
        if (getBit(lcdc, 5) && isIntersectAtWindow(ly, state.x)) {
            uint8_t id = getWindowColorIdAt(ly, state.x);
            uint8_t color = getPaletteColor(bgp, id);
            bgId = id, bgColor = color;
        }
    }
    lcd->setPixel(ly, state.x, bgColor);
    if (getBit(lcdc, 1)) {
        for (const auto& obj : state.scanlineObjects) {
            if (obj.isIntersectAtPoint(ly, state.x)) {
                uint8_t id = obj.getColorIdAt(ly, state.x);
                if (id == 0) continue;
                uint8_t palette = obj.getPalette(obp0, obp1);
                uint8_t color = getPaletteColor(palette, id);
                if (obj.isDrawn(bgId)) {
                    lcd->setPixel(ly, state.x, color);
                }
                break;
            }
        }
    }
    state.x++;
}

