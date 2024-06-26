#pragma once

#include <cstdint>

class Flags;

class ALU {
public:
	ALU(Flags& flags) : flags(flags) {}
	uint8_t add8(uint8_t lhs, uint8_t rhs, bool carry = false);
	uint8_t sub8(uint8_t lhs, uint8_t rhs, bool carry = true);
	uint8_t and8(uint8_t lhs, uint8_t rhs);
	uint8_t or8(uint8_t lhs, uint8_t rhs);
	uint8_t xor8(uint8_t lhs, uint8_t rhs);
	uint8_t inc8(uint8_t lhs);
	uint8_t dec8(uint8_t lhs);
	uint8_t rotLeft(uint8_t lhs, bool affect = true);
	uint8_t rotRight(uint8_t lhs, bool affect = true);
	uint8_t shiftLeft(uint8_t lhs);
	uint8_t shiftRight(uint8_t lhs, bool arith = true);
	uint8_t swap(uint8_t lhs);
private:
	Flags& flags;
};

