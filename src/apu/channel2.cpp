#include "channel2.h"
#include "address_bus.h"
#include "utils.h"

Channel2::Channel2(AddressBus& addrBus) {
    addrBus.setReader(0xff16, reg1);
    addrBus.setWriter(0xff16, [&](uint8_t byte) {
        reg1 = byte;
        lengthTimer = 64 - (reg1 & 63);
    });
    addrBus.setReader(0xff17, reg2);
    addrBus.setWriter(0xff17, [&](uint8_t byte) {
        reg2 = byte;
        volume = reg2 >> 4;
        volumeTicksLeft = reg2 & 0x7;
        setDacEnable(reg2 >> 3);
    });
    addrBus.setWriter(0xff18, reg3);
    addrBus.setReader(0xff19, reg4);
    addrBus.setWriter(0xff19, [&](uint8_t byte) {
        reg4 = byte;
        if (getBit(reg4, 7)) {
            if (dacEnable) {
                active = true;
            }
        }
    });
}

void Channel2::tickImpl() {
    dividerTicks++;
    if (dividerTicks == 1 && periodTicks == ((reg4 & 0x7) << 8 | reg3)) {
        digitalOutput = getBit(waveforms[reg1 >> 6], waveformIndex) ? volume : 0;
        waveformIndex = (waveformIndex + 1) % 8;
    }
    if (dividerTicks == 4) {
        periodTicks++;
        if (periodTicks >= 2048) {
            periodTicks = (reg4 & 0x7) << 8 | reg3;
        }
        dividerTicks = 0;
    }
}
