#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <filesystem>
#include <fstream>

class AddressBus;

enum class MapperKind : uint8_t {
    kNone,
    kMBC1,
    kMBC2,
    kMBC3,
    kMBC5
};

class Cartridge {
public:
    Cartridge(std::vector<uint8_t>& rom, uint32_t ramSize, const std::filesystem::path& path, bool hasBattery);
    virtual ~Cartridge() = default;
    virtual void loadToAddrBus(AddressBus& addrBus) = 0;
    virtual void serialize(std::ofstream& of) const = 0;
    virtual void deserialize(std::ifstream& is) = 0;
    void saveRAM() const;
    void loadRAM();
    std::filesystem::path getPath() const;
    static std::unique_ptr<Cartridge> create(MapperKind kind, std::vector<uint8_t> rom, uint32_t ramSize,
        std::filesystem::path path, bool hasBattery);
private:
    bool hasBattery;
    std::filesystem::path path;
protected:
    uint8_t readROM(uint32_t addr) const;
    uint8_t readRAM(uint32_t addr) const;
    void writeRAM(uint32_t addr, uint8_t byte);
    const std::vector<uint8_t> rom;
    std::vector<uint8_t> ram;
};