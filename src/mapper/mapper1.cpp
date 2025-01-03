#include "mapper1.h"

#include "address_bus.h"
#include "utils.h"

Mapper1::~Mapper1() {
    saveRAM();
}

void Mapper1::loadToAddrBus(AddressBus& addrBus) {
    for (int i = 0; i < 0x2000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            ramEnable = (byte & 0xf) == 0xa;
        });
    }
    for (int i = 0x2000; i < 0x4000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            byte &= 0x1f;
            bank1 = !byte ? 1 : byte;
        });
    }
    for (int i = 0x4000; i < 0x6000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            bank2 = byte & 0x3;
        });
    }
    for (int i = 0x6000; i < 0x8000; i++) {
        addrBus.setWriter(i, [&](uint8_t byte) {
            mode = byte & 0x1;
        });
    }
    for (int i = 0; i < 0x8000; i++) {
        addrBus.setReader(i, [&, i]() {
            return readROM(getROMAddress(i));
        });
    }
    for (int i = 0xa000; i < 0xc000; i++) {
        addrBus.setReader(i, [&, i]() {
            return ramEnable ? readRAM(getRAMAddress(i)) : 0xff;
        });
        addrBus.setWriter(i, [&, i](uint8_t byte) {
            if (ramEnable) {
                writeRAM(getRAMAddress(i), byte);
            }
        });
    }
}

void Mapper1::serialize(std::ofstream& of) const {
    using ::serialize;
    serialize(of, ram);
    serialize(of, ramEnable);
    serialize(of, bank1);
    serialize(of, bank2);
    serialize(of, mode);
}

void Mapper1::deserialize(std::ifstream& is) {
    using ::deserialize;
    deserialize(is, ram);
    deserialize(is, ramEnable);
    deserialize(is, bank1);
    deserialize(is, bank2);
    deserialize(is, mode);
}

uint32_t Mapper1::getROMAddress(uint16_t addr) const {
    uint8_t mask = (rom.size() >> 14) - 1;
    uint8_t bankNumber = 0;
    if (addr < 0x4000) {
        if (mode) {
            bankNumber = bank2 << 5;
        }
    } else {
        bankNumber = (bank2 << 5) | bank1;
    }
    bankNumber &= mask;
    addr &= (1 << 14) - 1;
    return (bankNumber << 14) | addr;
}

uint16_t Mapper1::getRAMAddress(uint16_t addr) const {
    addr &= (1 << 13) - 1;
    return ((bank2 * mode) << 13) | addr;
}
