#include "channel4.h"
#include "address_bus.h"
#include "utils.h"

Channel4::Channel4(AddressBus& addrBus) {
    addrBus.setWriter(0xff20, [&](uint8_t byte) {
        lengthTimer = 64 - byte;
    });
    addrBus.setReader(0xff21, reg2);
    addrBus.setWriter(0xff21, [&](uint8_t byte) {
        reg2 = byte;
        volume = reg2 >> 4;
        volumeTicksLeft = reg2 & 0x7;
        setDacEnable(reg2 >> 3);
    });
    addrBus.setReader(0xff22, reg3);
    addrBus.setWriter(0xff22, [&](uint8_t byte) {
        reg3 = byte;
        clockTicksLeft = divisors[reg3 & 0x7] << (reg3 >> 4);
    });
    addrBus.setReader(0xff23, reg4);
    addrBus.setWriter(0xff23, [&](uint8_t byte) {
        reg4 = byte;
        if (getBit(reg4, 7)) {
            if (dacEnable) {
                active = true;
            }
            lfsr = 0;
        }
    });
}

void Channel4::tickImpl() {
    clockTicksLeft--;
    if (!clockTicksLeft) {
        digitalOutput = shiftLFSR() ? volume : 0;
        clockTicksLeft = divisors[reg3 & 0x7] << (reg3 >> 4);
    }
}

bool Channel4::shiftLFSR() {
    bool res = (lfsr & 0x1) == (lfsr & 0x2);
    lfsr = setBit(lfsr, 15, res);
    if (getBit(reg3, 3)) {
        lfsr = setBit(lfsr, 7, res);
    }
    lfsr >>= 1;
    return lfsr & 0x1;
}
