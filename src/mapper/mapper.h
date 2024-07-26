#pragma once

#include <vector>
#include <cstdint>

class AddressBus;

class Mapper {
public:
    Mapper(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram) : rom(rom), ram(ram) {}
    virtual ~Mapper() = default;
    virtual void initBus(AddressBus& addrBus) = 0;
protected:
    const std::vector<uint8_t>& rom;
    std::vector<uint8_t>& ram;
};