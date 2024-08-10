#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include <memory>
#include <tuple>
#include "lcd.h"
#include "object_layer.h"

class AddressBus;
class IRQHandler;

class PPU {
public:
    PPU(AddressBus& addrBus, std::unique_ptr<LCD> lcd, IRQHandler& irqHandler);
    void reset();
    void tick();
private:
    enum Mode {
        kHBlank,
        kVBlank,
        kOAMScan,
        kDrawing,
        kDisabled = 0x4
    };
    struct State {
        std::vector<ObjectLayer> scanlineObjects;
        uint8_t x = 0;
        int16_t dmaIndex = -8;
        bool dmaActive = false;
        uint8_t source;
        bool wyCond = false, wxCond = false, wShown = false;
        uint8_t wl = 0;
    };
    void tickDMA();
    Tile getObjectTile(uint8_t index, uint8_t bank) const;
    BackgroundTile getNonObjectTile(uint8_t index, uint8_t attribute) const;
    BackgroundTile getTileAtTileMap1(uint8_t i, uint8_t j) const;
    BackgroundTile getTileAtTileMap2(uint8_t i, uint8_t j) const;
    BackgroundTile getWindowTileAt(uint8_t i, uint8_t j) const;
    BackgroundTile getBackgroundTileAt(uint8_t i, uint8_t j) const;
    std::tuple<uint8_t, Color, bool> getBackgroundColorAt(uint8_t i, uint8_t j) const;
    std::tuple<uint8_t, Color, bool> getWindowColorAt(uint8_t i, uint8_t j) const;

    ObjectLayer createObject(uint8_t index) const;

    uint8_t getPaletteColor(uint8_t palette, uint8_t id) const;
    void doLYCompare();
    void doSingleDotDrawing();
    // CGB

    Color getBGColor(uint8_t index, uint8_t colorId) const;
    Color getObjColor(uint8_t index, uint8_t colorId) const;


    AddressBus& addrBus;
    std::unique_ptr<LCD> lcd;
    IRQHandler& irqHandler;

    std::array<std::array<uint8_t, 1 << 13>, 2> vram{};
    std::array<uint8_t, 160> oam{};
    uint16_t tickCount = 0;
    uint8_t lcdc = 0, ly = 0, lyc = 0, scy = 0, scx = 0, wy = 0, wx = 0;
    uint8_t bgp = 0, obp0 = 0, obp1 = 0;
    uint8_t stat = 0;
    Mode currentMode = kDisabled, nextMode = kDisabled;
    State state;

    // CGB
    uint8_t vramBank = 0;
    std::array<uint8_t, 64> bgPaletteRAM{};
    uint8_t bgPaletteIndex = 0;

    std::array<uint8_t, 64> objPaletteRAM{};
    uint8_t objPaletteIndex = 0;

    bool isCGBMode = true;
};