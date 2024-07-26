#pragma once

#include <vector>
#include <cstdint>
#include <fstream>
#include <memory>
#include "mapper/mapper.h"

class AddressBus;

class Cartridge {
public:
    Cartridge(std::ifstream &file);
    void loadToAddrBus(AddressBus& addrBus);
private:
    std::vector<uint8_t> loadROM(std::ifstream& file);
    uint32_t ramSize() const;
    std::unique_ptr<Mapper> createMapper();

    const std::vector<uint8_t> rom;
    std::vector<uint8_t> ram;
    const std::unique_ptr<Mapper> mapper;
};