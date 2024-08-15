#pragma once

#include "cartridge.h"
#include <iostream>

class Mapper1 : public Cartridge {
public:
    Mapper1(std::vector<uint8_t>& rom, uint32_t ramSize, std::filesystem::path path, bool hasBattery) : Cartridge(rom, ramSize, path, hasBattery) {}
    ~Mapper1();
    void loadToAddrBus(AddressBus& addrBus) override;
    void serialize(std::ofstream& of) const override;
    void deserialize(std::ifstream& is) override;
private:
    uint32_t getROMAddress(uint16_t addr) const;
    uint16_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = true;
    uint8_t bank1 = 1, bank2 = 0, mode = 0;
};