#pragma once

#include "mapped.h"
#include <vector>
#include "utils.h"

class MBC1 : public MultiAddrMapped {
public:
    MBC1();
    void write(uint16_t addr, uint8_t byte) override;
    bool canAccessRAM() const {
        return (ramg.read() & 0xf) == 0b1010;
    }
    uint8_t bank1Value() const {
        return bank1.read() & 0x1f;
    }
    uint8_t bank2Value() const {
        return bank2.read() & 0x3;
    }
    bool modeValue() const {
        return mode.read() & 0x1;
    }
private:
    SingleAddrMapped ramg, bank1, bank2, mode;
    std::vector<uint8_t> rom;
};

