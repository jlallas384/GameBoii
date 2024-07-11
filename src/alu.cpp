#include "alu.h"
#include "flags.h"

uint8_t ALU::add(uint8_t lhs, uint8_t rhs) {
    return addImpl(lhs, rhs);
}

uint8_t ALU::adc(uint8_t lhs, uint8_t rhs) {
    return addImpl(lhs, rhs, flags.getC());
}

uint8_t ALU::sub(uint8_t lhs, uint8_t rhs) {
    return subImpl(lhs, rhs);
}

uint8_t ALU::sbc(uint8_t lhs, uint8_t rhs) {
    return subImpl(lhs, rhs, 1 - flags.getC());
}

uint8_t ALU::band(uint8_t lhs, uint8_t rhs) {
    uint8_t res = lhs & rhs;
    flags.setZ(res == 0);
    flags.setN(false);
    flags.setC(false);
    flags.setH(true);
    return res;
}

uint8_t ALU::bor(uint8_t lhs, uint8_t rhs) {
    uint8_t res = lhs | rhs;
    flags.setZ(res == 0);
    flags.setN(false);
    flags.setC(false);
    flags.setH(false);
    return res;
}

uint8_t ALU::bxor(uint8_t lhs, uint8_t rhs) {
    uint8_t res = lhs ^ rhs;
    flags.setZ(res == 0);
    flags.setN(false);
    flags.setC(false);
    flags.setH(false);
    return res;
}

uint8_t ALU::inc(uint8_t lhs) {
    bool c = flags.getC();
    uint8_t res = add(lhs, 1);
    flags.setC(c);
    return res;
}

uint8_t ALU::dec(uint8_t lhs) {
    bool c = flags.getC();
    uint8_t res = sub(lhs, 1);
    flags.setC(c);
    return res;
}

uint8_t ALU::rlc(uint8_t lhs) {
    return rotLeftImpl(lhs);
}

uint8_t ALU::rl(uint8_t lhs) {
    return rotLeftImpl(lhs, false);
}

uint8_t ALU::rrc(uint8_t lhs) {
    return rotRightImpl(lhs);
}

uint8_t ALU::rr(uint8_t lhs) {
    return rotRightImpl(lhs, false);
}

uint8_t ALU::sla(uint8_t lhs) {
    uint8_t res = lhs << 1;
    flags.setC(lhs & 0x80);
    flags.setZ(res == 0);
    flags.setN(false);
    flags.setH(false);
    return res;
}

uint8_t ALU::sra(uint8_t lhs) {
    return shiftRightImpl(lhs);
}

uint8_t ALU::srl(uint8_t lhs) {
    return shiftRightImpl(lhs, false);
}

uint8_t ALU::swap(uint8_t lhs) {
    uint8_t res = lhs << 4 | ((lhs & 0xf0) >> 4);
    flags.setC(false);
    flags.setZ(res == 0);
    flags.setN(false);
    flags.setH(false);
    return res;
}

uint8_t ALU::addImpl(uint8_t lhs, uint8_t rhs, bool carry) {
    int res = lhs + rhs + carry;
    flags.setZ((res & 0xff) == 0);
    flags.setN(false);
    flags.setC(res & 0x100);
    flags.setH((lhs & 0xf) + (rhs & 0xf) + carry & 0x10);
    return res;
}

uint8_t ALU::subImpl(uint8_t lhs, uint8_t rhs, bool carry) {
    uint8_t res = addImpl(lhs, ~rhs, carry);
    flags.setN(true);
    flags.setC(1 - flags.getC());
    flags.setH(1 - flags.getH());
    return res;
}

uint8_t ALU::rotLeftImpl(uint8_t lhs, bool affect) {
    bool newC = lhs & 0x80;
    bool oldC = flags.getC();
    uint8_t ret = (lhs << 1) | (affect ? newC : oldC);
    flags.setC(newC);
    flags.setZ(false);
    flags.setN(false);
    flags.setH(false);
    return ret;
}

uint8_t ALU::rotRightImpl(uint8_t lhs, bool affect) {
    bool newC = lhs & 0x1;
    bool oldC = flags.getC();
    uint8_t ret = (lhs >> 1) | (affect ? newC : oldC) << 7;
    flags.setC(newC);
    flags.setZ(false);
    flags.setN(false);
    flags.setH(false);
    return ret;
}

uint8_t ALU::shiftRightImpl(uint8_t lhs, bool arith) {
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
