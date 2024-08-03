#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include <memory>
#include "lcd.h"
#include "object_layer.h"

class AddressBus;
class IRQHandler;

class PPU {
public:
    PPU(AddressBus& addrBus, std::unique_ptr<LCD> lcd, IRQHandler& irqHandler);
    void tick();
public:
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
    };
    Tile getObjectTile(uint8_t index) const;
    Tile getNonObjectTile(uint8_t index) const;
    Tile getTileAtTileMap1(uint8_t i, uint8_t j) const;
    Tile getTileAtTileMap2(uint8_t i, uint8_t j) const;
    Tile getWindowTileAt(uint8_t i, uint8_t j) const;
    Tile getBackgroundTileAt(uint8_t i, uint8_t j) const;
    uint8_t getBackgroundColorIdAt(uint8_t i, uint8_t j) const;
    bool isIntersectAtWindow(uint8_t i, uint8_t j) const;
    uint8_t getWindowColorIdAt(uint8_t i, uint8_t j) const;

    ObjectLayer createObject(uint8_t index) const;
    std::vector<ObjectLayer> getObjectsToRender() const;

    uint8_t getPaletteColor(uint8_t palette, uint8_t id) const;
    void doLYCompare();
    void doSingleDotDrawing();


    std::unique_ptr<LCD> lcd;
    IRQHandler& irqHandler;
    std::array<uint8_t, 1 << 13> vram{};
    std::array<uint8_t, 160> oam{};
    uint16_t tickCount = 0;
    uint8_t lcdc = 0, ly = 0, lyc = 0, scy = 0, scx = 0, wy = 0, wx = 0;
    uint8_t bgp = 0, obp0 = 0, obp1 = 0;
    uint8_t stat = 0;
    Mode currentMode = kDisabled, nextMode = kDisabled;
    State state;
};