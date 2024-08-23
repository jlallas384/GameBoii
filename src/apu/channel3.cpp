#include "channel3.h"
#include "address_bus.h"
#include "utils.h"

Channel3::Channel3(AddressBus& addrBus) {
    for (int i = 0; i < 16; i++) {
        addrBus.setReader(0xff30 + i, [&, i]() {
            return waveRAM[i];
        });
        addrBus.setWriter(0xff30 + i, [&, i](uint8_t byte) {
            waveRAM[i] = byte;
        });
    }
    addrBus.setReader(0xff1a, reg0);
    addrBus.setWriter(0xff1a, [&](uint8_t byte) {
        reg0 = byte;
        setDacEnable(getBit(reg0, 7));
    });
    addrBus.setWriter(0xff1b, [&](uint8_t byte) {
        lengthTimer = 256 - byte;
    });
    addrBus.setReader(0xff1c, reg2);
    addrBus.setWriter(0xff1c, reg2);
    addrBus.setWriter(0xff1d, reg3);
    addrBus.setReader(0xff1e, reg4);
    addrBus.setWriter(0xff1e, [&](uint8_t byte) {
        reg4 = byte;
        if (getBit(reg4, 7)) {
            if (dacEnable) {
                active = true;
            }
        }
    });
}

void Channel3::tickImpl() {
    dividerTicks++;
    if (dividerTicks == 1 && periodTicks == ((reg4 & 0x7) << 8 | reg3)) {
        digitalOutput = sampleAt(sampleIndex);
        uint8_t outputLevel = (reg2 >> 5) & 0x3;
        if (outputLevel) {
            digitalOutput >>= outputLevel - 1;
        } else {
            digitalOutput = 0;
        }
        sampleIndex = (sampleIndex + 1) % 32;
    }
    if (dividerTicks == 2) {
        periodTicks++;
        if (periodTicks >= 2048) {
            periodTicks = (reg4 & 0x7) << 8 | reg3;
        }
        dividerTicks = 0;
    }
}

uint8_t Channel3::sampleAt(uint8_t index) const {
    return index % 2 == 0 ? ((waveRAM[index / 2] & 0xf0) >> 4) : waveRAM[index / 2] & 0xf;
}