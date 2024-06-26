#pragma	once

#include <cstdint>
#include "flags.h"
#include "alu.h"

struct gb_int {
	gb_int() {}
	gb_int(int value) : high(static_cast<uint8_t>(value >> 8)), low(value & 0xFF) {}
	operator uint16_t() {
		return static_cast<uint16_t>((high << 8) | low);
	}
	uint8_t high = 0, low = 0;
};

class AddressBus;

class CPU {
public:
	CPU(AddressBus& _addrBus);
	void execute();
private:
	void executeCB();
	uint8_t fetch8();
	uint16_t fetch16();
	uint8_t& getRegister(uint8_t reg);
	gb_int& getRegisterPair(uint8_t reg, bool isSP = true);
	bool getCondition(uint8_t cc);

	gb_int af, bc, de, hl, pc, sp;
	bool ime = false;
	AddressBus& addrBus;
	Flags flags;
	ALU alu;
};

