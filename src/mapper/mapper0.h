#pragma once

#include "cartridge.h"

class Mapper0 : public Cartridge {
public:
    Mapper0(std::vector<uint8_t>& rom, uint32_t ramSize, std::filesystem::path path, bool hasBattery) : Cartridge(rom, ramSize, path, hasBattery) {}
    ~Mapper0();
    void loadToAddrBus(AddressBus& addrBus) override;
    void serialize(std::ofstream& of) const override;
    void deserialize(std::ifstream& is) override;
};

