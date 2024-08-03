#include "tile.h"
#include "utils.h"

Tile::Tile(std::span<const uint8_t> mem) : mem(mem) {

}

uint8_t Tile::at(uint8_t i, uint8_t j) const {
    auto index = i * 2;
    return getBit(mem[index], 7 - j) | (getBit(mem[index + 1], 7 - j) << 1);
}
