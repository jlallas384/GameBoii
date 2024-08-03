#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <filesystem>

class AddressBus;

enum class MapperKind {
    kNone,
    kMBC1,
    kMBC2
};

class Cartridge {
public:
    Cartridge(std::vector<uint8_t> &rom, uint32_t ramSize);
    virtual ~Cartridge();
    virtual void loadToAddrBus(AddressBus& addrBus) = 0;
    void loadRAM(std::filesystem::path path);
    static std::unique_ptr<Cartridge> create(MapperKind kind, std::vector<uint8_t> rom, uint32_t ramSize);
private:
    std::filesystem::path savePath;
protected:
    uint8_t readROM(uint32_t addr) const;
    uint8_t readRAM(uint32_t addr) const;
    void writeRAM(uint32_t addr, uint8_t byte);
    const std::vector<uint8_t> rom;
    std::vector<uint8_t> ram;
};