#pragma once

#include <cstdint>

class CPU;
class AddressBus;

class IRQHandler {
public:
    enum RequestKind {
        kVBlank,
        kStat,
        kTimer,
        kSerial,
        KJoypad
    };
    IRQHandler(CPU& cpu, AddressBus& addrBus);
    void handle();
    void request(RequestKind rk);
private:
    CPU& cpu;
    uint8_t intEnable = 0, intFlag = 0;
};