#include "mapper2.h"

#include "address_bus.h"
#include "utils.h"

Mapper2::~Mapper2() {
    saveRAM();
}

void Mapper2::loadToAddrBus(AddressBus& addrBus) {
    for (int i = 0; i < 0x4000; i++) {
        AddressBus::Writer w;
        if (!getBit(i, 8)) {
            w = [&](uint8_t byte) {
                ramEnable = (byte & 0xf) == 0xa;
            };
        } else {
            w = [&](uint8_t byte) {
                byte &= 0xf;
                bank = !byte ? 1 : byte;
            };
        }
        addrBus.setWriter(i, w);
    }
    for (int i = 0; i < 0x8000; i++) {
        addrBus.setReader(i, [&, i]() {
            return readROM(getROMAddress(i));
        });
    }
    for (int i = 0xa000; i < 0xc000; i++) {
        addrBus.setReader(i, [&, i]() {
            return ramEnable ? readRAM(getRAMAddress(i)) & 0xf : 0xff;
        });
        addrBus.setWriter(i, [&, i](uint8_t byte) {
            if (ramEnable) {
                writeRAM(getRAMAddress(i), byte & 0xf);
            }
        });
    }
}

void Mapper2::serialize(std::ofstream& of) const {
    using ::serialize;
    serialize(of, ram);
    serialize(of, ramEnable);
    serialize(of, bank);
}

void Mapper2::deserialize(std::ifstream& is) {
    using ::deserialize;
    deserialize(is, ram);
    deserialize(is, ramEnable);
    deserialize(is, bank);
}

uint32_t Mapper2::getROMAddress(uint16_t addr) const {
    uint8_t bankNumber = addr < 0x4000 ? 0 : bank;
    addr &= (1 << 14) - 1;
    return (bankNumber << 14) | addr;
}

uint16_t Mapper2::getRAMAddress(uint16_t addr) const {
    addr &= (1 << 9) - 1;
    return addr;
}
