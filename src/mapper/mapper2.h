#pragma once

#include "mapper.h"

#include <vector>

class Mapper2 : public Mapper {
public:
    Mapper2(const std::vector<uint8_t>& rom) : Mapper(rom, ram), ram(512) {}
    void initBus(AddressBus& addrBus) override;
private:
    uint32_t getROMAddress(uint16_t addr) const;
    uint16_t getRAMAddress(uint16_t addr) const;
    std::vector<uint8_t> ram;
    bool ramEnable = false;
    uint8_t bank = 1;
};