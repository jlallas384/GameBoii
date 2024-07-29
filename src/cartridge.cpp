#include "cartridge.h"
#include <utility>
#include <memory>
#include <fstream>
#include "mapper/mapper0.h"
#include "mapper/mapper1.h"
#include "mapper/mapper2.h"
#include "address_bus.h"

Cartridge::Cartridge(std::vector<uint8_t> &rom, uint32_t ramSize) : rom(std::move(rom)), ram(ramSize) {

}

Cartridge::~Cartridge() {
    std::ofstream saveFile(savePath, std::ios::binary);
    saveFile.write(reinterpret_cast<char*>(ram.data()), ram.size());
}

void Cartridge::loadRAM(std::filesystem::path path) {
    if (std::filesystem::exists(path)) {
        std::ifstream saveFile(path);
        saveFile.read(reinterpret_cast<char*>(ram.data()), ram.size());
    }
    savePath = path;
}

std::unique_ptr<Cartridge> Cartridge::create(MapperKind kind, std::vector<uint8_t> rom, uint32_t ramSize) {
    switch (kind) {
        case MapperKind::kNone:
            return std::make_unique<Mapper0>(rom, ramSize);
        case MapperKind::kMBC1:
            return std::make_unique<Mapper1>(rom, ramSize);
        case MapperKind::kMBC2:
            return std::make_unique<Mapper2>(rom);
        default:
            return nullptr;
    }
}