#pragma once
#include <cstdint>

class AddressBus {
public:
	void write(uint16_t addr, uint8_t byte);
	uint8_t read(uint16_t addr);
private:
	int mem[100000];
};
