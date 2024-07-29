#pragma once

#include "cartridge.h"

class Mapper1 : public Cartridge {
public:
    Mapper1(std::vector<uint8_t>& rom, uint32_t ramSize) : Cartridge(rom, ramSize) {}
    void loadToAddrBus(AddressBus& addrBus) override;
public:
    uint32_t getROMAddress(uint16_t addr) const;
    uint16_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = true;
    uint8_t bank1 = 1, bank2 = 0, mode = 0;
};