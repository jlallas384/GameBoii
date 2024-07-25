#pragma once

#include <cstdint>

class Flags;

class ALU {
public:
    ALU(Flags& flags) : flags(flags) {}
    uint8_t add(uint8_t lhs, uint8_t rhs);
    uint8_t adc(uint8_t lhs, uint8_t rhs);
    uint8_t sub(uint8_t lhs, uint8_t rhs);
    uint8_t sbc(uint8_t lhs, uint8_t rhs);
    uint8_t band(uint8_t lhs, uint8_t rhs);
    uint8_t bor(uint8_t lhs, uint8_t rhs);
    uint8_t bxor(uint8_t lhs, uint8_t rhs);
    uint8_t inc(uint8_t lhs);
    uint8_t dec(uint8_t lhs);
    uint8_t rlc(uint8_t lhs);
    uint8_t rl(uint8_t lhs);
    uint8_t rrc(uint8_t lhs);
    uint8_t rr(uint8_t lhs);
    uint8_t sla(uint8_t lhs);
    uint8_t sra(uint8_t lhs);
    uint8_t srl(uint8_t lhs);
    uint8_t swap(uint8_t lhs);
private:
    uint8_t addImpl(uint8_t lhs, uint8_t rhs, bool carry = false);
    uint8_t subImpl(uint8_t lhs, uint8_t rhs, bool carry = true);
    uint8_t rotLeftImpl(uint8_t lhs, bool affect = true);
    uint8_t rotRightImpl(uint8_t lhs, bool affect = true);
    uint8_t shiftRightImpl(uint8_t lhs, bool arith = true);
    Flags& flags;
};

