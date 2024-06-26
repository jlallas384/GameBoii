#include <cstdint>
#include <iostream>
#include "cpu.h"
#include "addressbus.h"

void print(Flags& f) {
	std::cout << "Z: " << f.getZ() << " C: " << f.getC() << " H: " << f.getH() << std::endl;
}



int main() {
	uint8_t x = 0;
	Flags f(x);
	ALU alu(f);
	uint8_t xx = alu.add8(255, 1);
	uint8_t zz = alu.shiftLeft(128);
	print(f);
	std::cout << (int)zz;
}
