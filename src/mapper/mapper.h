#pragma once

#include <vector>
#include <cstdint>
#include <memory>

class AddressBus;

class Mapper {
public:
    Mapper(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram) : rom(rom), ram(ram) {}
    virtual ~Mapper() = default;
    virtual void initBus(AddressBus& addrBus) = 0;
    static std::unique_ptr<Mapper> create(uint8_t type, const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram);
protected:
    const std::vector<uint8_t>& rom;
    std::vector<uint8_t>& ram;
};