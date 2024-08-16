#pragma	once

#include <cstdint>
#include "flags.h"
#include "alu.h"

struct gb_int {
    gb_int() = default;
    gb_int(int value) : high(static_cast<uint8_t>(value >> 8)), low(value & 0xff) {}
    operator uint16_t() const {
        return static_cast<uint16_t>((high << 8) | low);
    }
    uint8_t high = 0, low = 0;
};

class AddressBus;

class CPU {
public:
    CPU(AddressBus& addrBus);
    bool isDoubleSpeed() const;
    void tick();
    void reset();
    void serialize(std::ofstream& of) const;
    void deserialize(std::ifstream& is);
private:
    void execute();
    void executeCB();
    uint8_t fetch8();
    uint16_t fetch16();
    uint8_t& getRegister(uint8_t reg);
    gb_int& getRegisterPair(uint8_t reg);
    bool getCondition(uint8_t cc) const;
    void call(uint16_t addr);

    gb_int af, bc, de, hl, pc, sp;
    bool ime = false, halted = false, doubleSpeed = false;
    AddressBus& addrBus;
    Flags flags;
    ALU alu;
    uint8_t ticksLeft = 0;
    friend class IRQHandler;
};

