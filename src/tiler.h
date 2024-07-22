#pragma once
#include "mapped.h"
#include <vector>
#include <cstdint>

class Tile;

class Tiler : public MultiAddrMapped {
public:
    Tiler();
    Tile getTileSigned(int8_t index);
    Tile getTileUnsigned(uint8_t index);
private:
    std::vector<SingleAddrMapped> vram;
};

