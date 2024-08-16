#pragma once

#include "cartridge.h"

#include <vector>

class Mapper2 : public Cartridge {
public:
    Mapper2(std::vector<uint8_t>& rom, const std::filesystem::path& path, bool hasBattery) : Cartridge(rom, 512, path, hasBattery) {}
    ~Mapper2() override;
    void loadToAddrBus(AddressBus& addrBus) override;
    void serialize(std::ofstream& of) const override;
    void deserialize(std::ifstream& is) override;
private:
    uint32_t getROMAddress(uint16_t addr) const;
    uint16_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = false;
    uint8_t bank = 1;
};