#pragma once

#include "cartridge.h"

#include <vector>

class Mapper2 : public Cartridge {
public:
    Mapper2(std::vector<uint8_t>& rom) : Cartridge(rom, 512) {}
    void loadToAddrBus(AddressBus& addrBus) override;
private:
    uint32_t getROMAddress(uint16_t addr) const;
    uint16_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = false;
    uint8_t bank = 1;
};