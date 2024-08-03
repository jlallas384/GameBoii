#include "irq_handler.h"

#include "cpu/cpu.h"
#include "utils.h"
#include "address_bus.h"

IRQHandler::IRQHandler(CPU& cpu, AddressBus& addrBus) : cpu(cpu) { 
    addrBus.setReader(0xffff, intEnable);
    addrBus.setWriter(0xffff, [&](uint8_t byte) {
        intEnable = byte & 0x1f;
    });
    addrBus.setReader(0xff0f, intFlag);
    addrBus.setWriter(0xff0f, [&](uint8_t byte) {
        intFlag = byte & 0x1f;
    });
}

void IRQHandler::handle() {
    if (!cpu.ime) return;
    for (int i = 0; i < 5; i++) {
        if (getBit(intEnable, i) && getBit(intFlag, i)) {
            cpu.halted = false;
            cpu.ime = false;
            intFlag = setBit(intFlag, i, false);
            return cpu.call(0x40 + 0x8 * i);
        }
    }
}

void IRQHandler::request(RequestKind rk) {
    intFlag = setBit(intFlag, rk, true);
}