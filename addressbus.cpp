#include "addressbus.h"

void AddressBus::write(uint16_t addr, uint8_t byte) {
	mem[addr] = byte;
}

uint8_t AddressBus::read(uint16_t addr) {
	return mem[addr];
}
