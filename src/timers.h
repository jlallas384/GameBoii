#pragma once

#include <cstdint>

class AddressBus;
class IRQHandler;

class Timers {
public:
    Timers(AddressBus& addrBus, IRQHandler& IRQHandler);
    void tick();
private:
    IRQHandler& irqHandler;
    bool previousAND = false;
    uint16_t div = 0;
    uint8_t tac = 0, tima = 0, tma = 0;
};