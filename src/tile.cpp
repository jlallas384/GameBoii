#include "tile.h"
#include <vector>
#include "utils.h"

Tile::Tile(std::span<const uint8_t> mem) {
    for (int i = 0; i < 16; i += 2) {
        int row = i / 2;
        for (int bit = 0; bit < 8; bit++) {
            arr[row] = setBit(arr[row], bit * 2 + 1, getBit(mem[i], 7 - bit));
        }
    }
    for (int i = 1; i < 16; i += 2) {
        int row = i / 2;
        for (int bit = 0; bit < 8; bit++) {
            arr[row] = setBit(arr[row], bit * 2, getBit(mem[i], 7 - bit));
        }
    }
}

uint8_t Tile::at(uint8_t i, uint8_t j) {
    uint8_t val = arr[i] >> (j * 2) & 0x3;
    return ((val & 0x1) << 1) | ((val & 0x2) >> 1);
}