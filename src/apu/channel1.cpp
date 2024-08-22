#include "channel1.h"
#include "address_bus.h"
#include "utils.h"

Channel1::Channel1(AddressBus& addrBus) {
    addrBus.setReader(0xff10, reg0);
    addrBus.setWriter(0xff10, reg0);
    addrBus.setReader(0xff11, reg1);
    addrBus.setWriter(0xff11, [&](uint8_t byte) {
        reg1 = byte;
        lengthTimer = 64 - (reg1 & 63);
    });
    addrBus.setReader(0xff12, reg2);
    addrBus.setWriter(0xff12, [&](uint8_t byte) {
        reg2 = byte;
        volume = reg2 >> 4;
        volumeTicksLeft = reg2 & 0x7;
        setDacEnable(reg2 >> 3);
    });
    addrBus.setWriter(0xff13, reg3);
    addrBus.setReader(0xff14, reg4);
    addrBus.setWriter(0xff14, [&](uint8_t byte) {
        reg4 = byte;
        if (getBit(reg4, 7)) {
            if (dacEnable) {
                active = true;
            }
            ticksLeft = (reg0 >> 4) & 0x7;
        }
    });
}

void Channel1::tickSweep() {
    if (ticksLeft == 0) {
        ticksLeft = (reg0 >> 4) & 0x7;
    }
    uint16_t currentPeriod = (reg4 & 0x7) << 8 | reg3;
    int nextPeriod = currentPeriod;
    uint16_t diff = currentPeriod >> (reg0 & 0x7);
    if (getBit(reg0, 3)) {
        nextPeriod -= diff;
    } else {
        nextPeriod += diff;
    }
    if (nextPeriod > 0x7ff) {
        active = false;
    }
    if (ticksLeft) {
        ticksLeft--;
        if (!ticksLeft) {
            uint16_t period = nextPeriod & 0xffff;
            reg3 = period & 0xff;
            reg4 = (reg4 & ~0x7) | (period >> 8);
        }
    }
}

void Channel1::tickImpl() {
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
