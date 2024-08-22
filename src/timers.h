#pragma once

#include <cstdint>
#include <fstream>

class AddressBus;
class IRQHandler;
class APU;

class Timers {
public:
    Timers(AddressBus& addrBus, IRQHandler& irqHandler, APU& apu);
    void tick(bool isDoubleSpeed);
    void serialize(std::ofstream& of) const;
    void deserialize(std::ifstream& is);
private:
    IRQHandler& irqHandler;
    APU& apu;
    bool previousAND = false;
    uint16_t div = 0;
    uint8_t tac = 0, tima = 0, tma = 0;
};