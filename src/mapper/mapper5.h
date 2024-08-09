#pragma once

#include "cartridge.h"

#include <vector>

class Mapper5 : public Cartridge {
public:
    Mapper5(std::vector<uint8_t>& rom, uint32_t ramSize) : Cartridge(rom, ramSize) {}
    void loadToAddrBus(AddressBus& addrBus) override;
private:
    uint32_t getROMAddress(uint16_t addr) const;
    uint32_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = true;
    uint8_t romBank1 = 0, romBank2 = 0, ramBank = 0;
};