#include "cartridge.h"
#include <utility>
#include <memory>
#include <fstream>
#include "mapper/mapper0.h"
#include "mapper/mapper1.h"
#include "mapper/mapper2.h"
#include "mapper/mapper3.h"
#include "address_bus.h"

Cartridge::Cartridge(std::vector<uint8_t> &rom, uint32_t ramSize) : rom(std::move(rom)), ram(ramSize) {

}

Cartridge::~Cartridge() {
    if (!savePath.empty()) {
        std::ofstream saveFile(savePath, std::ios::binary);
        saveFile.write(reinterpret_cast<char*>(ram.data()), ram.size());
    }
}

void Cartridge::loadRAM(std::filesystem::path path) {
    if (std::filesystem::exists(path)) {
        std::ifstream saveFile(path, std::ios::binary);
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
        case MapperKind::kMBC3:
            return std::make_unique<Mapper3>(rom, ramSize);
        default:
            return nullptr;
    }
}

uint8_t Cartridge::readROM(uint32_t addr) const {
    return addr < rom.size() ? rom[addr] : 0xff;
}

uint8_t Cartridge::readRAM(uint32_t addr) const {
    return addr < ram.size() ? ram[addr] : 0xff;
}

void Cartridge::writeRAM(uint32_t addr, uint8_t byte) {
    if (addr < ram.size()) {
        ram[addr] = byte;
    }
}
