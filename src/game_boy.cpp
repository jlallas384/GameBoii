#include "game_boy.h"
#include <vector>
#include <cstdint>
#include <fstream>
#include "cartridge.h"

GameBoy::GameBoy() : cpu(addrBus) {

}

uint32_t getRamSize(uint8_t value) {
    switch (value) {
        case 0x0:
        case 0x1:
            return 0;
        case 0x2:
            return 8192;
        case 0x3:
            return 32768;
        case 0x4:
            return 131072;
        case 0x5:
            return 65536;
        default:
            return 0;
            // todo error;
    }
}

std::unique_ptr<Cartridge> createCartidge(uint8_t value, std::vector<uint8_t>& rom, uint32_t ramSize) {
    switch (value) {
        case 0x0:
            return Cartridge::create(MapperKind::kNone, rom, ramSize);
            break;
        case 0x1:
        case 0x2:
        case 0x3:
            return Cartridge::create(MapperKind::kMBC1, rom, ramSize);
            break;
        case 0x4:
        case 0x5:
            return Cartridge::create(MapperKind::kMBC2, rom, ramSize);
            break;
        case 0x11:
        case 0x12:
        case 0x13:
            return Cartridge::create(MapperKind::kNone, rom, ramSize);
            break;
        default:
            return nullptr;
    }
}

void GameBoy::loadCartridge(std::filesystem::path path) {
    std::ifstream file(path);
    file.seekg(0x148);
    auto value = file.peek(); // TODO error check
    auto romSize = 32768 * (1 << value);
    std::vector<uint8_t> rom(romSize);
    file.read(reinterpret_cast<char*>(rom.data()), romSize);
    auto ramSize = getRamSize(rom[0x149]);
    cartridge = createCartidge(rom[0x147], rom, ramSize);
    cartridge->loadRAM(path.replace_extension("sav"));
    cartridge->loadToAddrBus(addrBus);
}

