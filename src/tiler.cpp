#include "tiler.h"
#include "tile.h"

Tiler::Tiler() : vram(0x1800) {
    for (int i = 0x8000; i <= 0x97ff; i++) {
        setRange(i, i, &vram[i]);
    }
}

Tile Tiler::getTileSigned(int8_t index) {
    return Tile(std::span<uint8_t>(vram.begin(), 16));
}

Tile Tiler::getTileUnsigned(uint8_t index) {
    return Tile();
}
