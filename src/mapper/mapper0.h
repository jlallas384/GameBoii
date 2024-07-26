#pragma once

#include "mapper.h"

class Mapper0 : public Mapper {
public:
    Mapper0(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram) : Mapper(rom, ram) {}
    void initBus(AddressBus& addrBus) override;
};

