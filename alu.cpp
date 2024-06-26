#include "alu.h"
#include "flags.h"

uint8_t ALU::add8(uint8_t lhs, uint8_t rhs, bool carry) {
	int res = lhs + rhs + carry;
	flags.setZ((res & 0xff) == 0);
	flags.setN(false);
	flags.setC(res & 0x100);
	flags.setH((lhs & 0xf) + (rhs & 0xf) + carry & 0x10);
	return res;
}

uint8_t ALU::sub8(uint8_t lhs, uint8_t rhs, bool carry) {
	uint8_t res = add8(lhs, ~rhs, carry);
	flags.setN(true);
	flags.setC(1 - flags.getC());
	flags.setH(1 - flags.getH());
	return res;
}

uint8_t ALU::and8(uint8_t lhs, uint8_t rhs) {
	uint8_t res = lhs & rhs;
	flags.setZ(res == 0);
	flags.setN(false);
	flags.setC(false);
	flags.setH(true);
	return res;
}

uint8_t ALU::or8(uint8_t lhs, uint8_t rhs) {
	uint8_t res = lhs | rhs;
	flags.setZ(res == 0);
	flags.setN(false);
	flags.setC(false);
	flags.setH(false);
	return res;
}

uint8_t ALU::xor8(uint8_t lhs, uint8_t rhs) {
	uint8_t res = lhs ^ rhs;
	flags.setZ(res == 0);
	flags.setN(false);
	flags.setC(false);
	flags.setH(false);
	return res;
}

uint8_t ALU::inc8(uint8_t lhs) {
	bool c = flags.getC();
	uint8_t res = add8(lhs, 1);
	flags.setC(c);
	return res;
}

uint8_t ALU::dec8(uint8_t lhs) {
	bool c = flags.getC();
	uint8_t res = sub8(lhs, 1);
	flags.setC(c);
	return res;
}

uint8_t ALU::rotLeft(uint8_t lhs, bool affect) {
	bool newC = lhs & 0x80;
	bool oldC = flags.getC();
	uint8_t ret = (lhs << 1) | (affect ? newC : oldC);
	flags.setC(newC);
	flags.setZ(false);
	flags.setN(false);
	flags.setH(false);
	return ret;
}

uint8_t ALU::rotRight(uint8_t lhs, bool affect) {
	bool newC = lhs & 0x1;
	bool oldC = flags.getC();
	uint8_t ret = (lhs >> 1) | (affect ? newC : oldC) << 7;
	flags.setC(newC);
	flags.setZ(false);
	flags.setN(false);
	flags.setH(false);
	return ret;
}

uint8_t ALU::shiftLeft(uint8_t lhs) {
	uint8_t res = lhs << 1;
	flags.setC(lhs & 0x80);
	flags.setZ(res == 0);
	flags.setN(false);
	flags.setH(false);
	return res;
}

uint8_t ALU::shiftRight(uint8_t lhs, bool arith) {
	uint8_t res = lhs >> 1;
	if (arith) {
		res |= (lhs & 0x80);
	}
	flags.setC(lhs & 0x1);
	flags.setZ(res == 0);
	flags.setN(false);
	flags.setH(false);
	return res;
}

uint8_t ALU::swap(uint8_t lhs) {
	uint8_t res = lhs << 4 | ((lhs & 0xf0) >> 4);
	flags.setC(false);
	flags.setZ(res == 0);
	flags.setN(false);
	flags.setH(false);
	return res;
}
