#pragma once

#include "cartridge.h"

#include <vector>
#include <iostream>

class Mapper2 : public Cartridge {
public:
    Mapper2(std::vector<uint8_t>& rom, std::filesystem::path path, bool hasBattery) : Cartridge(rom, 512, path, hasBattery) {}
    void loadToAddrBus(AddressBus& addrBus) override;
private:
    void serializeImpl(std::ofstream& of) const override;
    uint32_t getROMAddress(uint16_t addr) const;
    uint16_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = false;
    uint8_t bank = 1;
};