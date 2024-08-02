#include "tile.h"
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

uint8_t Tile::at(uint8_t i, uint8_t j) const {
    uint8_t val = arr[i] >> (j * 2) & 0x3;
    return ((val & 0x1) << 1) | ((val & 0x2) >> 1);
}

//int main() {
//    std::vector<uint8_t> a = { 0x3C ,0x7E ,0x42 ,0x42 ,0x42 ,0x42 ,0x42 ,0x42 ,0x7E ,0x5E ,0x7E ,0x0A ,0x7C ,0x56 ,0x38 ,0x7C };
//    Tile t(std::span<uint8_t>(a.begin(), 16));
//    for (int i = 0; i < 8; i++) {
//        for (int j = 0; j < 8; j++) {
//            std::cout << (int)t.at(i, j) << ' ';
//        }
//        std::cout << '\n';
//    }
//}