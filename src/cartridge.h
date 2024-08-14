#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <filesystem>
#include <fstream>

class AddressBus;

enum class MapperKind {
    kNone,
    kMBC1,
    kMBC2,
    kMBC3,
    kMBC5
};

class Cartridge {
public:
    Cartridge(std::vector<uint8_t>& rom, uint32_t ramSize, std::filesystem::path, bool hasBattery);
    virtual ~Cartridge();
    virtual void loadToAddrBus(AddressBus& addrBus) = 0;
    void serialize(std::ofstream& of) const;
    std::filesystem::path getPath() const;
    static std::unique_ptr<Cartridge> create(MapperKind kind, std::vector<uint8_t> rom, uint32_t ramSize,
        std::filesystem::path path, bool hasBattery);
private:
    virtual void serializeImpl(std::ofstream& of) const = 0;
    std::filesystem::path path;
    bool hasBattery;
protected:
    uint8_t readROM(uint32_t addr) const;
    uint8_t readRAM(uint32_t addr) const;
    void writeRAM(uint32_t addr, uint8_t byte);
    const std::vector<uint8_t> rom;
    std::vector<uint8_t> ram;
};