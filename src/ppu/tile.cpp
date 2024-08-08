#include "tile.h"
#include "utils.h"
#include <cassert>
Tile::Tile(std::span<const uint8_t> mem) : mem(mem) {

}

uint8_t Tile::at(uint8_t i, uint8_t j) const {
    auto index = i * 2;
    return getBit(mem[index], 7 - j) | (getBit(mem[index + 1], 7 - j) << 1);
}

BackgroundTile::BackgroundTile(std::span<const uint8_t> mem, uint8_t attribute) : Tile(mem) {
    priority = getBit(attribute, 7);
    flipY = getBit(attribute, 6);
    flipX = getBit(attribute, 5);
    palette = attribute & 0x7;
}

uint8_t BackgroundTile::at(uint8_t i, uint8_t j) const {
    if (flipY) {
        i = 7 - i;
    }
    if (flipX) {
        j = 7 - j;
    }
    return Tile::at(i, j);
}

uint8_t BackgroundTile::getPalette() const {
    return palette;
}
