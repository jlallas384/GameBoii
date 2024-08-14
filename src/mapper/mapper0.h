#pragma once

#include "cartridge.h"

class Mapper0 : public Cartridge {
public:
    Mapper0(std::vector<uint8_t>& rom, uint32_t ramSize, std::filesystem::path path, bool hasBattery) : Cartridge(rom, ramSize, path, hasBattery) {}
    void loadToAddrBus(AddressBus& addrBus) override;
private:
    void serializeImpl(std::ofstream& of) const override {}
};

