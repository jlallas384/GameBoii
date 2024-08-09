#include "mapper5.h"

#include "address_bus.h"

void Mapper5::loadToAddrBus(AddressBus& addrBus) {
    for (int i = 0; i < 0x2000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            ramEnable = (byte & 0xf) == 0xa;
        });
    }
    for (int i = 0x2000; i < 0x3000; i++) {
        addrBus.setWriter(i, romBank1);
    }
    for (int i = 0x3000; i < 0x4000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            romBank2 = byte & 0x1;
        });
    }
    for (int i = 0x4000; i < 0x6000; i++) {
        addrBus.setWriter(i, ramBank);
    }

    for (int i = 0; i < 0x4000; i++) {
        addrBus.setReader(i, [&, i]() {
            return readROM(i);
        });
    }
    for (int i = 0x4000; i < 0x8000; i++) {
        addrBus.setReader(i, [&, i]() {
            return readROM(getROMAddress(i));
        });
    }
    for (int i = 0xa000; i < 0xc000; i++) {
        addrBus.setReader(i, [&, i]() {
            return ramEnable ? readRAM(getRAMAddress(i)) : 0xff;
        });
        addrBus.setWriter(i, [&, i](uint8_t byte) {
            if (ramEnable) writeRAM(getRAMAddress(i), byte);
        });
    }
}

uint32_t Mapper5::getROMAddress(uint16_t addr) const {
    uint8_t mask = (rom.size() >> 14) - 1;
    uint8_t bankNumber = (romBank1 | (romBank2 << 8)) & mask;
    addr &= (1 << 14) - 1;
    return (bankNumber << 14) | addr;
}

uint32_t Mapper5::getRAMAddress(uint16_t addr) const {
    addr &= (1 << 13) - 1;
    return (ramBank << 13) | addr;
}
