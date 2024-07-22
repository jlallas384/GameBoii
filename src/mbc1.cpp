#include "mbc1.h"

MBC1::MBC1() : bank1(0x1) {
    setRange(0x0000, 0x1fff, &ramg);
    setRange(0x2000, 0x3fff, &bank1);
    setRange(0x4000, 0x5fff, &bank2);
    setRange(0x6000, 0x7fff, &mode);
}

void MBC1::write(uint16_t addr, uint8_t byte) {
    if (addr < 0xa000 || canAccessRAM()) {
        MultiAddrMapped::write(addr, byte);
    }
    if (bank1Value() == 0) {
        bank1.write(0x1);
    }
    
}
