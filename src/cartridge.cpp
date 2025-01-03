#include "cartridge.h"
#include <utility>
#include <memory>
#include <fstream>
#include "utils.h"
#include "mapper/mapper0.h"
#include "mapper/mapper1.h"
#include "mapper/mapper2.h"
#include "mapper/mapper3.h"
#include "mapper/mapper5.h"

Cartridge::Cartridge(std::vector<uint8_t> &rom, uint32_t ramSize, const std::filesystem::path& path, bool hasBattery) : hasBattery(hasBattery), path(path), rom(std::move(rom)), ram(ramSize) {

}

void Cartridge::saveRAM() const {
    if (hasBattery) {
        std::ofstream saveFile(getPath().replace_extension("sav"), std::ios::binary);
        serialize(saveFile);
    }
}

void Cartridge::loadRAM() {
    if (const auto savePath = getPath().replace_extension("sav"); exists(savePath)) {
        std::ifstream saveFile(savePath, std::ios::binary);
        deserialize(saveFile);
    }
}

std::filesystem::path Cartridge::getPath() const {
    return path;
}

std::unique_ptr<Cartridge> Cartridge::create(MapperKind kind, std::vector<uint8_t> rom, uint32_t ramSize, const std::filesystem::path& path, bool hasBattery) {
    switch (kind) {
        case MapperKind::kNone:
            return std::make_unique<Mapper0>(rom, ramSize, path, hasBattery);
        case MapperKind::kMBC1:
            return std::make_unique<Mapper1>(rom, ramSize, path, hasBattery);
        case MapperKind::kMBC2:
            return std::make_unique<Mapper2>(rom, path, hasBattery);
        case MapperKind::kMBC3:
            return std::make_unique<Mapper3>(rom, ramSize, path, hasBattery);
        case MapperKind::kMBC5:
            return std::make_unique<Mapper5>(rom, ramSize, path, hasBattery);
    }
    return nullptr;
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
