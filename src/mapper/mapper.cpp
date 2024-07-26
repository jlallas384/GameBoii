#include "mapper.h"
#include "mapper0.h"
#include "mapper1.h"
#include "mapper2.h"

std::unique_ptr<Mapper> Mapper::create(uint8_t type, const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram) {
    switch (type) {
        case 0:
            return std::make_unique<Mapper0>(rom, ram);
        case 1:
            return std::make_unique<Mapper1>(rom, ram);
        case 2:
            return std::make_unique<Mapper2>(rom);
        default:
            return nullptr;
    }
}
    