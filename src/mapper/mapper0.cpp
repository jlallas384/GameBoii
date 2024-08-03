#include "mapper0.h"
#include "address_bus.h"

void Mapper0::loadToAddrBus(AddressBus& addrBus) {
    for (int i = 0; i < rom.size(); i++) {
        addrBus.setReader(i, [&, i]() {
            return readROM(i);
        });
    }
    for (int i = 0; i < ram.size(); i++) {
        addrBus.setReader(i + 0xa000, [&, i]() {
            return readRAM(i);
        });
        addrBus.setWriter(i + 0xa000, [&, i](uint8_t byte) {
            writeRAM(i, byte);
        });
    }
}
