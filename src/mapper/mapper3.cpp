#include "mapper3.h"

#include "address_bus.h"

void Mapper3::loadToAddrBus(AddressBus& addrBus) {
    for (int i = 0; i < 0x2000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            ramEnable = (byte & 0xf) == 0xa;
        });
    }
    for (int i = 0x2000; i < 0x4000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            romBank = !byte ? 1 : byte;
        });
    }
    for (int i = 0x4000; i < 0x6000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            ramBank = byte;
        });
    }
    for (int i = 0xa000; i < 0xc000; i++) {
        addrBus.setReader(i, [&, i]() {
            if (ramBank < 4) {
                return ramEnable ? readRAM(getRAMAddress(i)) : static_cast<uint8_t>(0xff);
            } else {
                return rtc;
            }
        });
        addrBus.setWriter(i, [&, i](uint8_t byte) {
            if (ramBank < 4) {
                if (ramEnable) writeRAM(getRAMAddress(i), byte);
            } else {
                rtc = byte;
            }
        });
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
}

uint32_t Mapper3::getROMAddress(uint16_t addr) const {
    uint8_t mask = (rom.size() >> 14) - 1;
    uint8_t bankNumber = romBank & mask;
    addr &= (1 << 14) - 1;
    return (bankNumber << 14) | addr;
}

uint16_t Mapper3::getRAMAddress(uint16_t addr) const {
    addr &= (1 << 13) - 1;
    return (ramBank << 13) | addr;
}
