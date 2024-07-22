#include "irq_handler.h"

#include "cpu.h"
#include "utils.h"

IRQHandler::IRQHandler(CPU& cpu) : cpu(cpu) {

}

void IRQHandler::handle() {
    if (!cpu.ime) return;
    for (int i = 0; i < 5; i++) {
        if (getBit(intEnable->read(), i) && getBit(intFlag->read(), i)) {
            cpu.ime = false;
            intFlag->writeBit(i, false);
            return cpu.call(0x40 + 0x8 * i);
        }
    }
}

void IRQHandler::requestVBlank() {
    intFlag->writeBit(0, true);
}

void IRQHandler::requestStat() {
    intFlag->writeBit(1, true);
}

void IRQHandler::requestTimer() {
    intFlag->writeBit(2, true);
}

void IRQHandler::requestSerial() {
    intFlag->writeBit(3, true);
}

void IRQHandler::requestJoypad() {
    intFlag->writeBit(4, true);
}