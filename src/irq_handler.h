#pragma once

#include <cstdint>
#include <iostream>

class CPU;
class AddressBus;

class IRQHandler {
public:
    enum RequestKind : uint8_t {
        kVBlank,
        kStat,
        kTimer,
        kSerial,
        kJoypad
    };
    IRQHandler(CPU& cpu, AddressBus& addrBus);
    void handle();
    void request(RequestKind rk);
    void serialize(std::ofstream& of) const;
    void deserialize(std::ifstream& is);
private:
    CPU& cpu;
    uint8_t intEnable = 0, intFlag = 0;
};