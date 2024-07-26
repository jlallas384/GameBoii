#include "mapper2.h"

#include "address_bus.h"
#include "utils.h"

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
            return rom[getROMAddress(i)];
        });
    }
    for (int i = 0xa000; i < 0xc000; i++) {
        addrBus.setReader(i, [&, i]() {
            return ramEnable ? ram[getRAMAddress(i)] & 0xf : 0xff;
        });
        addrBus.setWriter(i, [&, i](uint8_t byte) {
            if (ramEnable) {
                ram[getRAMAddress(i)] = byte & 0xf;
            }
        });
    }
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