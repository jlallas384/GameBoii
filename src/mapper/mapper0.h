#pragma once

#include "cartridge.h"

class Mapper0 : public Cartridge {
public:
    Mapper0(std::vector<uint8_t>& rom, uint32_t ramSize) : Cartridge(rom, ramSize) {}
    void loadToAddrBus(AddressBus& addrBus) override;
};

