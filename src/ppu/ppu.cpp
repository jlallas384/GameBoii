#include "ppu.h"
#include <algorithm>
#include "irq_handler.h"
#include "tile.h"
#include "address_bus.h"
#include "utils.h"
#include "object_layer.h"
#include <iostream>

PPU::PPU(AddressBus& addrBus, std::unique_ptr<LCD> lcd, IRQHandler& irqHandler) : addrBus(addrBus), lcd(std::move(lcd)), irqHandler(irqHandler) {
    for (int i = 0; i < (1 << 13); i++) {
        addrBus.setReader(0x8000 + i, [&, i]() {
            return currentMode != kDrawing ? vram[vramBank][i] : 0xff;
        });
        addrBus.setWriter(0x8000 + i, [&, i](uint8_t byte) {
            if (currentMode != kDrawing) {
                vram[vramBank][i] = byte;
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
        state.source = byte;
        state.dmaActive = true;
        state.dmaIndex = -8;
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


    // CGB
    addrBus.setReader(0xff4f, vramBank);
    addrBus.setWriter(0xff4f, [&](uint8_t byte) {
        vramBank = byte & 0x1;
    });

    addrBus.setReader(0xff68, bgPaletteIndex);
    addrBus.setWriter(0xff68, bgPaletteIndex);

    addrBus.setReader(0xff69, [&]() {
        return currentMode != kDrawing ? bgPaletteRAM[bgPaletteIndex & 63] : 0xff;
    });
    addrBus.setWriter(0xff69, [&](uint8_t byte) {
        if (currentMode != kDrawing) {
            bgPaletteRAM[bgPaletteIndex & 63] = byte;
        }
        if (getBit(bgPaletteIndex, 7)) {
            bgPaletteIndex = (1 << 7) | (((bgPaletteIndex & 63) + 1) & 63);
        }
    });

    addrBus.setReader(0xff6a, objPaletteIndex);
    addrBus.setWriter(0xff6a, objPaletteIndex);

    addrBus.setReader(0xff6b, [&]() {
        return currentMode != kDrawing ? objPaletteRAM[objPaletteIndex & 63] : 0xff;
    });
    addrBus.setWriter(0xff6b, [&](uint8_t byte) {
        if (currentMode != kDrawing) {
            objPaletteRAM[objPaletteIndex & 63] = byte;
        }
        if (getBit(objPaletteIndex, 7)) {
            objPaletteIndex = (1 << 7) | (((objPaletteIndex & 63) + 1) & 63);
        }
    });

    for (int i = 0xff51; i <= 0xff55; i++) {
        addrBus.setReader(i, [&]() {
            std::cout << "tried reading\n";
            return 0;
        });
        addrBus.setWriter(i, [&](uint8_t byte) {
            std::cout << "tried writinging\n";
        });
    }
}

void PPU::reset() {
    state.x = 0;
    state.wxCond = false;
    state.wyCond = false;
    state.dmaActive = false;
    currentMode = kDisabled;
    nextMode = kDisabled;
    stat = 0;
    tickCount = 0;
    lcdc = 0;
}

// TODO clean this
void PPU::tick() {
    if (state.dmaActive) {
        state.dmaIndex++;
        if (state.dmaIndex >= 0 && state.dmaIndex % 4 == 0) {
            oam[state.dmaIndex / 4] = addrBus.read((state.source << 8) | (state.dmaIndex / 4));
            if (state.dmaIndex / 4 == oam.size() - 1) {
                state.dmaActive = false;
            }
        }
    }
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
                state.scanlineObjects.clear();
                doLYCompare();
                state.wyCond |= wy == ly;
                state.wxCond = false;
                state.wShown = false;
            }
            if (tickCount % 2 == 0 && state.scanlineObjects.size() < 10) {
                auto object = createObject(tickCount / 2 - 1);
                if (object.isAtScanline(ly)) {
                    state.scanlineObjects.push_back(object);
                }
            }
            if (tickCount == 80) {
                nextMode = kDrawing;
            }
            break;
        case kDrawing:
            if (tickCount == 1) {
                std::stable_sort(state.scanlineObjects.begin(), state.scanlineObjects.end());
            }
            if (state.x < LCD::width) {
                state.wxCond |= state.x + 7 == wx;
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
                    if (state.wShown) {
                        state.wl++;
                    }
                    nextMode = kOAMScan;
                } else {
                    nextMode = kVBlank;
                }
            }
            break;
        case kVBlank:
            if (tickCount == 1) {
                doLYCompare();
                if (ly == 144) {
                    if (getBit(stat, 4)) {
                        irqHandler.request(IRQHandler::kStat);
                    }
                    irqHandler.request(IRQHandler::kVBlank);
                    lcd->refresh();
                }
            }
            if (tickCount == 456) {
                ly++;
                tickCount = 0;
            }
            if (ly == 154) {
                nextMode = kOAMScan;
                state.wyCond = false;
                state.wl = 0;
                ly = 0;
            }
            break;
        case kDisabled:
            break;
    }
}

Tile PPU::getObjectTile(uint8_t index, uint8_t bank) const {
    return Tile(std::span(vram[bank].begin() + index * 16, 16));
}

Tile PPU::getNonObjectTile(uint8_t index, uint8_t bank) const {
    return getBit(lcdc, 4) ? Tile(std::span(vram[bank].begin() + index * 16, 16))
        : Tile(std::span(vram[bank].begin() + 0x1000 + static_cast<int8_t>(index) * 16, 16));
}

Tile PPU::getTileAtTileMap1(uint8_t i, uint8_t j) const {
    auto index = 0x1800 + i * 32 + j;
    return getNonObjectTile(vram[0][index], 0); // todo bank from attribute
}

Tile PPU::getTileAtTileMap2(uint8_t i, uint8_t j) const {
    auto index = 0x1c00 + i * 32 + j;
    return getNonObjectTile(vram[0][index], 0); // todo bank from attribute
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

uint8_t PPU::getWindowColorIdAt(uint8_t i, uint8_t j) const {
    j -= wx - 7;
    Tile t = getWindowTileAt(i / 8, j / 8);
    return t.at(i % 8, j % 8);
}

ObjectLayer PPU::createObject(uint8_t index) const {
    index *= 4;
    ObjectData data = { oam[index], oam[index + 1], oam[index + 2], oam[index + 3] };
    if (!getBit(lcdc, 2)) {
        return ObjectLayer(getObjectTile(data.tileIndex, 0), data);
    } else {
        Tile t1 = getObjectTile(data.tileIndex & 0xfe, 0);
        Tile t2 = getObjectTile(data.tileIndex | 0x1, 0);
        return ObjectLayer(t1, t2, data);
    }
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
        if (getBit(lcdc, 5) && state.wxCond && state.wyCond) {
            state.wShown = true;
            uint8_t id = getWindowColorIdAt(state.wl, state.x);
            uint8_t color = getPaletteColor(bgp, id);
            bgId = id, bgColor = color;
        }
    }
    lcd->setPixel(ly, state.x, getBGColor(0, bgColor));
    if (getBit(lcdc, 1)) {
        for (const auto& obj : state.scanlineObjects) {
            if (obj.isIntersectAtPoint(ly, state.x)) {
                uint8_t id = obj.getColorIdAt(ly, state.x);
                if (id == 0) continue;
                bool paletteBool = obj.getDMGPalette();
                uint8_t palette = paletteBool ? obp1 : obp0;
                uint8_t color = getPaletteColor(palette, id);
                if (obj.isDrawn(bgId)) {
                    lcd->setPixel(ly, state.x, getObjColor(paletteBool, color));
                }
                break;
            }
        }
    }
    state.x++;
}

uint16_t PPU::getBGColor(uint8_t index, uint8_t colorId) {
    return bgPaletteRAM[index * 8 + colorId * 2] | (bgPaletteRAM[index * 8 + colorId * 2 + 1] << 8);
}

uint16_t PPU::getObjColor(uint8_t index, uint8_t colorId) {
    return objPaletteRAM[index * 8 + colorId * 2] | (objPaletteRAM[index * 8 + colorId * 2 + 1] << 8);
}

