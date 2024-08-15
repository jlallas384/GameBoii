#pragma once

#include "cartridge.h"

#include <vector>
#include <iostream>

class Mapper5 : public Cartridge {
public:
    Mapper5(std::vector<uint8_t>& rom, uint32_t ramSize, std::filesystem::path path, bool hasBattery) : Cartridge(rom, ramSize, path, hasBattery) {}
    ~Mapper5();
    void loadToAddrBus(AddressBus& addrBus) override;
    void serialize(std::ofstream& of) const override;
    void deserialize(std::ifstream& is) override;
private:
    uint32_t getROMAddress(uint16_t addr) const;
    uint32_t getRAMAddress(uint16_t addr) const;
    bool ramEnable = true;
    uint8_t romBank1 = 0, romBank2 = 0, ramBank = 0;
};