#pragma once

#include "mapper.h"

class Mapper1 : public Mapper {
public:
    Mapper1(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram) : Mapper(rom, ram) {}
    void initBus(AddressBus& addrBus) override;
public:
    uint32_t getROMAddress(uint16_t addr) const;
    uint16_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = true;
    uint8_t bank1 = 1, bank2 = 0, mode = 0;
};