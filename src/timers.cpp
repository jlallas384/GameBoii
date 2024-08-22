#include "timers.h"
#include <cstdint>
#include "irq_handler.h"
#include "address_bus.h"
#include "utils.h"
#include "apu/apu.h"

Timers::Timers(AddressBus& addrBus, IRQHandler& irqHandler, APU& apu) : irqHandler(irqHandler), apu(apu) {
    addrBus.setReader(0xff04, [&]() {
        return div >> 8;
    });
    addrBus.setWriter(0xff04, [&](uint8_t byte) {
        div = 0;
    });
    addrBus.setReader(0xff05, tima);
    addrBus.setWriter(0xff05, tima);
    addrBus.setReader(0xff06, tma);
    addrBus.setWriter(0xff06, tma);
    addrBus.setReader(0xff07, tac);
    addrBus.setWriter(0xff07, tac);
}

void Timers::tick(bool isDoubleSpeed) {
    uint16_t previousDIV = div;
    div++;
    uint8_t bitPos = 13 + isDoubleSpeed;
    if (getBit(previousDIV, bitPos) && !getBit(div, bitPos)) {
        apu.divTick();
    }
    static uint8_t pos[] = { 9, 3, 5, 7 };
    bitPos = pos[tac & 0x3];
    bool currentAND = getBit(tac, 2) && getBit(div, bitPos);
    if (!currentAND && previousAND) {
        tima++;
        if (tima == 0) {
            tima = tma;
            irqHandler.request(IRQHandler::kTimer);
        }
    }
    previousAND = currentAND;
}

void Timers::serialize(std::ofstream& of) const {
    using ::serialize;
    serialize(of, previousAND);
    serialize(of, div);
    serialize(of, tac);
    serialize(of, tac);
    serialize(of, tima);
    serialize(of, tma);
}

void Timers::deserialize(std::ifstream& is) {
    using ::deserialize;
    deserialize(is, previousAND);
    deserialize(is, div);
    deserialize(is, tac);
    deserialize(is, tac);
    deserialize(is, tima);
    deserialize(is, tma);
}
