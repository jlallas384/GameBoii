#include "irq_handler.h"

#include "cpu/cpu.h"
#include "utils.h"
#include "address_bus.h"

IRQHandler::IRQHandler(CPU& cpu, AddressBus& addrBus) : cpu(cpu) { 
    addrBus.setReader(0xffff, intEnable);
    addrBus.setWriter(0xffff, intEnable);
    addrBus.setReader(0xff0f, [&]() {
        return intFlag | 0xe0;
    });
    addrBus.setWriter(0xff0f, intFlag);
}

void IRQHandler::handle() {
    for (int i = 0; i < 5; i++) {
        if (getBit(intEnable, i) && getBit(intFlag, i)) {
            cpu.halted = false;
            if (cpu.ime) {
                cpu.ime = false;
                intFlag = setBit(intFlag, i, false);
                return cpu.call(0x40 + 0x8 * i);
            }
        }
    }
}

void IRQHandler::request(RequestKind rk) {
    intFlag = setBit(intFlag, rk, true);
}

void IRQHandler::serialize(std::ofstream& of) const {
    using ::serialize;
    serialize(of, intEnable);
    serialize(of, intFlag);
}
