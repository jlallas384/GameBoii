#include "ppu.h"
#include <algorithm>
#include "irq_handler.h"
#include "tile.h"
#include "address_bus.h"
#include "utils.h"
#include "object_layer.h"

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
            state.wl = 0;
            state.x = 0;
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

    addrBus.setReader(0xff68, backgroundPaletteIndex);
    addrBus.setWriter(0xff68, backgroundPaletteIndex);

    addrBus.setReader(0xff69, [&]() {
        return currentMode != kDrawing ? backgroundPaletteRAM[backgroundPaletteIndex & 63] : 0xff;
    });
    addrBus.setWriter(0xff69, [&](uint8_t byte) {
        if (currentMode != kDrawing) {
            backgroundPaletteRAM[backgroundPaletteIndex & 63] = byte;
        }
        if (getBit(backgroundPaletteIndex, 7)) {
            backgroundPaletteIndex = (1 << 7) | (((backgroundPaletteIndex & 63) + 1) & 63);
        }
    });

    addrBus.setReader(0xff6a, objectPaletteIndex);
    addrBus.setWriter(0xff6a, objectPaletteIndex);

    addrBus.setReader(0xff6b, [&]() {
        return currentMode != kDrawing ? objectPaletteRAM[objectPaletteIndex & 63] : 0xff;
    });
    addrBus.setWriter(0xff6b, [&](uint8_t byte) {
        if (currentMode != kDrawing) {
            objectPaletteRAM[objectPaletteIndex & 63] = byte;
        }
        if (getBit(objectPaletteIndex, 7)) {
            objectPaletteIndex = (1 << 7) | (((objectPaletteIndex & 63) + 1) & 63);
        }
    });

    addrBus.setWriter(0xff51, [&](uint8_t byte) {
        hdmaSource = (hdmaSource & 0xff) | (byte << 8);
    });
    addrBus.setWriter(0xff52, [&](uint8_t byte) {
        hdmaSource = (hdmaSource & 0xff00) | (byte & 0xf0);
    });
    addrBus.setWriter(0xff53, [&](uint8_t byte) {
        hdmaDest = (hdmaDest & 0xff) | ((byte & 0x1f) << 8);
    });
    addrBus.setWriter(0xff54, [&](uint8_t byte) {
        hdmaDest = (hdmaDest & 0xff00) | (byte & 0xf0);
    });

    addrBus.setReader(0xff55, [&]() {
        return !state.hdmaLength ? 0xff : state.hdmaLength / 0x10 - 1;
    });
    addrBus.setWriter(0xff55, [&](uint8_t byte) {
        state.hdmaLength = ((byte & 0x7f) + 1) * 0x10; //TODO disabling
        if (!getBit(byte, 7)) {
            while (state.hdmaLength) {
                tickHDMA();
            }
        }
    });

    addrBus.setWriter(0xff4c, [&](uint8_t byte) {
        isCGBMode = byte != 0x4;
    });
}

void PPU::reset() {
    state.x = 0;
    state.wxCond = false;
    state.wyCond = false;
    state.dmaActive = false;
    state.hdmaLength = 0;
    currentMode = kDisabled;
    nextMode = kDisabled;
    stat = 0;
    tickCount = 0;
    lcdc = 0;
    isCGBMode = true;
    wy = 0;
    wx = 0;
    scx = 0;
    scy = 0;
}

// TODO clean this
void PPU::tick(bool isDoubleSpeed) {
    tickDMA();
    if (isDoubleSpeed) {
        tickDMA();
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
            doOAMScanMode();
            break;
        case kDrawing:
            doDrawingMode();
            break;
        case kHBlank:
            doHBlankMode();
            break;
        case kVBlank:
            doVBlankMode();
            break;
        case kDisabled:
            break;
    }
}

void PPU::serialize(std::ofstream& of) const {
    using ::serialize;
    serialize(of, vram);
    serialize(of, oam);
    serialize(of, tickCount);
    serialize(of, lcdc);
    serialize(of, ly);
    serialize(of, lyc);
    serialize(of, scy);
    serialize(of, scx);
    serialize(of, wy);
    serialize(of, wx);
    serialize(of, bgp);
    serialize(of, obp0);
    serialize(of, obp1);
    serialize(of, stat);
    serialize(of, vramBank);
    serialize(of, backgroundPaletteRAM);
    serialize(of, backgroundPaletteIndex);
    serialize(of, objectPaletteRAM);
    serialize(of, objectPaletteIndex);
    serialize(of, hdmaSource);
    serialize(of, hdmaDest);
    state.serialize(of);
}

void PPU::deserialize(std::ifstream& is) {
    using ::deserialize;
    deserialize(is, vram);
    deserialize(is, oam);
    deserialize(is, tickCount);
    deserialize(is, lcdc);
    deserialize(is, ly);
    deserialize(is, lyc);
    deserialize(is, scy);
    deserialize(is, scx);
    deserialize(is, wy);
    deserialize(is, wx);
    deserialize(is, bgp);
    deserialize(is, obp0);
    deserialize(is, obp1);
    deserialize(is, stat);
    deserialize(is, vramBank);
    deserialize(is, backgroundPaletteRAM);
    deserialize(is, backgroundPaletteIndex);
    deserialize(is, objectPaletteRAM);
    deserialize(is, objectPaletteIndex);
    deserialize(is, hdmaSource);
    deserialize(is, hdmaDest);
    state.deserialize(is);
}

void PPU::tickDMA() {
    if (state.dmaActive) {
        state.dmaIndex++;
        if (state.dmaIndex >= 0 && state.dmaIndex % 4 == 0) {
            oam[state.dmaIndex / 4] = addrBus.read((state.source << 8) | (state.dmaIndex / 4));
            if (state.dmaIndex / 4 == oam.size() - 1) {
                state.dmaActive = false;
            }
        }
    }
}

void PPU::tickHDMA() {
    if (!state.hdmaLength) return;
    std::generate_n(vram[vramBank].begin() + hdmaDest, 16, [&]() {
        return addrBus.read(hdmaSource++);
    });
    hdmaDest += 16;
    state.hdmaLength -= 16;
}

Tile PPU::getObjectTile(uint8_t index, uint8_t bank) const {
    return { std::span(vram[bank].begin() + index * 16, 16) };
}

BackgroundTile PPU::getNonObjectTile(uint8_t index, uint8_t attribute) const {
    uint8_t bank = getBit(attribute, 3);
    return getBit(lcdc, 4) ? BackgroundTile(std::span(vram[bank].begin() + index * 16, 16), attribute)
        : BackgroundTile(std::span(vram[bank].begin() + 0x1000 + static_cast<int8_t>(index) * 16, 16), attribute);
}

BackgroundTile PPU::getTileAtTileMap1(uint8_t i, uint8_t j) const {
    auto index = 0x1800 + i * 32 + j;
    uint8_t attribute = isCGBMode ? vram[1][index] : 0;
    return getNonObjectTile(vram[0][index], attribute);
}

BackgroundTile PPU::getTileAtTileMap2(uint8_t i, uint8_t j) const {
    auto index = 0x1c00 + i * 32 + j;
    uint8_t attribute = isCGBMode ? vram[1][index] : 0;
    return getNonObjectTile(vram[0][index], attribute);
}

BackgroundTile PPU::getWindowTileAt(uint8_t i, uint8_t j) const {
    return getBit(lcdc, 6) ? getTileAtTileMap2(i, j) : getTileAtTileMap1(i, j);
}

BackgroundTile PPU::getBackgroundTileAt(uint8_t i, uint8_t j) const {
    return getBit(lcdc, 3) ? getTileAtTileMap2(i, j) : getTileAtTileMap1(i, j);
}

std::tuple<uint8_t, Color, bool> PPU::getBackgroundColorAt(uint8_t i, uint8_t j) const {
    i = (i + scy) % 256, j = (j + scx) % 256;
    BackgroundTile t = getBackgroundTileAt(i / 8, j / 8);
    uint8_t id = t.at(i % 8, j % 8);
    if (isCGBMode) {
        return { id, getBackgroundColor(t.getPalette(), id), t.getPriority() };
    }
    return { id, getBackgroundColor(0, getDMGPaletteColor(bgp, id)), t.getPriority() };
}

std::tuple<uint8_t, Color, bool> PPU::getWindowColorAt(uint8_t i, uint8_t j) const {
    j -= wx - 7;
    BackgroundTile t = getWindowTileAt(i / 8, j / 8);
    uint8_t id = t.at(i % 8, j % 8);
    if (isCGBMode) {
        return { id, getBackgroundColor(t.getPalette(), id), t.getPriority() };
    }
    return { id, getBackgroundColor(0, getDMGPaletteColor(bgp, id)), t.getPriority() };
}

ObjectLayer PPU::createObject(uint8_t index) const {
    index *= 4;
    ObjectData data = { oam[index], oam[index + 1], oam[index + 2], oam[index + 3] };
    uint8_t bank = getBit(data.attributes, 3);
    if (!getBit(lcdc, 2)) {
        return { getObjectTile(data.tileIndex, bank), data };
    } else {
        Tile t1 = getObjectTile(data.tileIndex & 0xfe, bank);
        Tile t2 = getObjectTile(data.tileIndex | 0x1, bank);
        return { t1, t2, data };
    }
}

uint8_t PPU::getDMGPaletteColor(uint8_t palette, uint8_t id) {
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
    uint8_t bgId = 0;
    Color bgColor = LCD::white;
    bool bgPriority = false;
    if (getBit(lcdc, 0) || isCGBMode) {
        std::tie(bgId, bgColor, bgPriority) = getBackgroundColorAt(ly, state.x);
        if (getBit(lcdc, 5) && state.wxCond && state.wyCond) {
            state.wShown = true;
            std::tie(bgId, bgColor, bgPriority) = getWindowColorAt(state.wl, state.x);
        }
    }
    lcd->setPixel(ly, state.x, bgColor);
    if (getBit(lcdc, 1)) {
        const auto iter = std::ranges::find_if(state.scanlineObjects, [this](const auto& obj) {
            return obj.isIntersectAtPoint(ly, state.x) && obj.getColorIdAt(ly, state.x) != 0;
        });
        if (iter != state.scanlineObjects.end()) {
            const auto& obj = *iter;
            uint8_t id = obj.getColorIdAt(ly, state.x);
            if (isCGBMode) {
                if (bgId == 0 || !getBit(lcdc, 0) || (!bgPriority && !obj.getPriority())) {
                    lcd->setPixel(ly, state.x, getObjectColor(obj.getCGBPalette(), id));
                }
            } else {
                if (!obj.getPriority() || bgId == 0) {
                    uint8_t palette = obj.getDMGPalette() ? obp1 : obp0;
                    lcd->setPixel(ly, state.x, getObjectColor(obj.getDMGPalette(), getDMGPaletteColor(palette, id)));
                }
            }
        }
    }
    state.x++;
}

void PPU::doOAMScanMode() {
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
        if (const auto object = createObject(tickCount / 2 - 1); object.isAtScanline(ly)) {
            state.scanlineObjects.push_back(object);
        }
    }
    if (tickCount == 80) {
        nextMode = kDrawing;
    }
}

void PPU::doDrawingMode() {
    if (tickCount == 1 && !isCGBMode) {
        std::stable_sort(state.scanlineObjects.begin(), state.scanlineObjects.end());
    }
    if (state.x < LCD::width) {
        state.wxCond |= state.x + 7 == wx;
        doSingleDotDrawing();
    }
    if (tickCount == 172) {
        nextMode = kHBlank;
    }
}

void PPU::doHBlankMode() {
    if (tickCount == 1) {
        if (getBit(stat, 3)) {
            irqHandler.request(IRQHandler::kStat);
        }
        tickHDMA();
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
}

void PPU::doVBlankMode() {
    if (tickCount == 1) {
        doLYCompare();
        if (ly == 144) {
            lcd->refresh();
            if (getBit(stat, 4)) {
                irqHandler.request(IRQHandler::kStat);
            }
            irqHandler.request(IRQHandler::kVBlank);
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
}

Color PPU::getBackgroundColor(uint8_t index, uint8_t colorId) const {
    return backgroundPaletteRAM[index * 8 + colorId * 2] | (backgroundPaletteRAM[index * 8 + colorId * 2 + 1] << 8);
}

Color PPU::getObjectColor(uint8_t index, uint8_t colorId) const {
    return objectPaletteRAM[index * 8 + colorId * 2] | (objectPaletteRAM[index * 8 + colorId * 2 + 1] << 8);
}

void PPU::State::serialize(std::ofstream& of) const {
    using ::serialize;
    serialize(of, dmaIndex);
    serialize(of, dmaActive);
    serialize(of, source);
    serialize(of, hdmaLength);
}

void PPU::State::deserialize(std::ifstream& is) {
    using ::deserialize;
    deserialize(is, dmaIndex);
    deserialize(is, dmaActive);
    deserialize(is, source);
    deserialize(is, hdmaLength);
}
