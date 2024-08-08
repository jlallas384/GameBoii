#pragma once

#include <cstdint>
#include "tile.h"

struct ObjectData {
    uint8_t y;
    uint8_t x;
    uint8_t tileIndex;
    uint8_t attributes;
};

class ObjectLayer {
public:
    ObjectLayer(const Tile& t1, const ObjectData& data);
    ObjectLayer(const Tile& t1, const Tile& t2, const ObjectData& data);

    bool isAtScanline(uint8_t ly) const;
    bool isIntersectAtPoint(uint8_t i, uint8_t j) const;
    uint8_t getColorIdAt(uint8_t i, uint8_t j) const;
    uint8_t getDMGPalette() const;
    uint8_t getCGBPalette() const;
    bool isDrawn(uint8_t bgId) const;
    bool operator<(const ObjectLayer& other) const;
private:
    void initAttributes(const ObjectData& data);
    Tile t1, t2;
    uint8_t height, y, x;
    bool priority, flipY, flipX;
    uint8_t dmgPalette, cgbPalette;
};