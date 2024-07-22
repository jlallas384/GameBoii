#pragma once

#include <memory>
#include <cstdint>
#include "mapped.h"

class CPU;

class IRQHandler {
public:
    IRQHandler(CPU& cpu);
    void handle();
    void requestVBlank();
    void requestStat();
    void requestTimer();
    void requestSerial();
    void requestJoypad();
private:
    CPU& cpu;
    std::unique_ptr<SingleAddrMapped> intEnable, intFlag;
};