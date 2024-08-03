#include "timers.h"
#include <cstdint>
#include "irq_handler.h"
#include "address_bus.h"
#include "utils.h"

Timers::Timers(AddressBus& addrBus, IRQHandler& irqHandler) : irqHandler(irqHandler) {
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

void Timers::tick() {
    div++;
    static uint8_t pos[] = { 9, 3, 5, 7 };
    uint8_t bitPos = pos[tac & 0x3];
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
