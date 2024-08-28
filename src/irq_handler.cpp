#include "irq_handler.h"
#include <bit>
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
    const auto index = std::countr_zero(static_cast<uint8_t>(intEnable & intFlag));
    if (index <= kJoypad) {
        cpu.halted = false;
        if (cpu.ime) {
            cpu.ime = false;
            intFlag = setBit(intFlag, index, false);
            return cpu.call(0x40 + 0x8 * index);
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

void IRQHandler::deserialize(std::ifstream& is) {
    using ::deserialize;
    deserialize(is, intEnable);
    deserialize(is, intFlag);
}
