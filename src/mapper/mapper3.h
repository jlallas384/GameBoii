#pragma once

#include "cartridge.h"

#include <vector>

class Mapper3 : public Cartridge {
public:
    Mapper3(std::vector<uint8_t>& rom, uint32_t ramSize) : Cartridge(rom, ramSize) {}
    void loadToAddrBus(AddressBus& addrBus) override;
private:
    uint32_t getROMAddress(uint16_t addr) const;
    uint16_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = true;
    uint8_t romBank = 1, ramBank = 0, rtc = 0;
};