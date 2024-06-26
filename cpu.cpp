#include "cpu.h"
#include <exception>
#include "addressbus.h"

CPU::CPU(AddressBus& addrBus): addrBus(addrBus), flags(af.low), alu(flags) {

}

void CPU::execute() {
	uint8_t op = fetch8();
	switch (op) {
		case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4a:
		case 0x4b:
		case 0x4c:
		case 0x4d:
		case 0x4f:
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55:
		case 0x57:
		case 0x58:
		case 0x59:
		case 0x5a:
		case 0x5b:
		case 0x5c:
		case 0x5d:
		case 0x5f:
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		case 0x64:
		case 0x65:
		case 0x67:
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
		case 0x6c:
		case 0x6d:
		case 0x6f:
		case 0x78:
		case 0x79:
		case 0x7a:
		case 0x7b:
		case 0x7c:
		case 0x7d:
		case 0x7f: {
			uint8_t &dest = getRegister(op >> 3 & 0x7), src = getRegister(op & 0x7);
			dest = src;
			break;
		}
		case 0x6:
		case 0xe:
		case 0x16:
		case 0x1e:
		case 0x26:
		case 0x2e:
		case 0x3e: {
			uint8_t imme = fetch8();
			uint8_t &dest = getRegister(op >> 3 & 0x7);
			dest = imme;
			break;
		}
		case 0x46:
		case 0x4e:
		case 0x56:
		case 0x5e:
		case 0x66:
		case 0x6e:
		case 0x7e: {
			uint8_t byte = addrBus.read(hl);
			uint8_t& dest = getRegister(op >> 3 & 0x7);
			dest = byte;
			break;
		}
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x77: {
			uint8_t byte = getRegister(op & 0x7);
			addrBus.write(hl, byte);
			break;
		}
		case 0x36: {
			uint8_t imme = fetch8();
			addrBus.write(hl, imme);
			break;
		}
		case 0xa: {
			uint8_t byte = addrBus.read(bc);
			af.high = byte;
			break;
		}
		case 0x1a: {
			uint8_t byte = addrBus.read(de);
			af.high = byte;
			break;
		}
		case 0xf2: {
			uint8_t byte = addrBus.read(0xff00 + bc.low);
			af.high = byte;
			break;
		}
		case 0xe2: {
			uint8_t byte = af.high;
			addrBus.write(0xff00 + bc.low, byte);
			break;
		}
		case 0xf0: {
			uint8_t imme = fetch8();
			uint8_t byte = addrBus.read(0xff00 + imme);
			af.high = byte;
			break;
		}
		case 0xe0: {
			uint8_t imme = fetch8();
			addrBus.write(0xff00 + imme, af.high);
			break;
		}
		case 0xfa: {
			uint16_t addr = fetch16();
			uint8_t byte = addrBus.read(addr);
			af.high = byte;
			break;
		}
		case 0xea: {
			uint16_t addr = fetch16();
			addrBus.write(addr, af.high);
			break;
		}
		case 0x2a: {
			uint8_t byte = addrBus.read(hl);
			af.high = byte;
			hl = hl + 1;
			break;
		}
		case 0x3a: {
			uint8_t byte = addrBus.read(hl);
			af.high = byte;
			hl = hl - 1;
			break;
		}
		case 0x2: {
			uint8_t byte = af.high;
			addrBus.write(bc, byte);
			break;
		}
		case 0x12: {
			uint8_t byte = af.high;
			addrBus.write(de, byte);
			break;
		}
		case 0x22: {
			uint8_t byte = af.high;
			addrBus.write(hl, byte);
			hl = hl + 1;
			break;
		}
		case 0x32: {
			uint8_t byte = af.high;
			addrBus.write(hl, byte);
			hl = hl - 1;
			break;
		}


		case 0x1:
		case 0x11:
		case 0x21:
		case 0x31: {
			gb_int& dest = getRegisterPair(op >> 4 & 0x3);
			uint16_t imme = fetch16();
			dest = imme;
			break;
		}
		case 0xf9: {
			sp = hl;
			break;
		}
		case 0xc5:
		case 0xd5:
		case 0xe5:
		case 0xf5: {
			gb_int src = getRegisterPair(op >> 4 & 0x3, false);
			addrBus.write(sp - 1, src.high);
			addrBus.write(sp - 2, src.low);
			sp = sp - 2;
			break;
		}
		case 0xc1:
		case 0xd1:
		case 0xe1:
		case 0xf1: {
			gb_int& dest = getRegisterPair(op >> 4 & 0x3, false);
			dest.low = addrBus.read(sp);
			dest.high = addrBus.read(sp + 1);
			sp = sp + 2;
			break;
		}
		case 0xf8: {
			int8_t byte = fetch8();
			if (byte >= 0) {
				alu.add8(sp, byte);
			} else {
				alu.sub8(sp, -byte);
			}
			hl = sp + byte;
			flags.setZ(false);
			flags.setN(false);
			break;
		}
		case 0x8: {
			uint16_t addr = fetch16();
			addrBus.write(addr, sp.low);
			addrBus.write(addr + 1, sp.high);
			break;
		}


		case 0x80:
		case 0x81:
		case 0x82:
		case 0x83:
		case 0x84:
		case 0x85:
		case 0x87: {
			uint8_t byte = getRegister(op & 0x7);
			af.high = alu.add8(af.high, byte);
			break;
		}
		case 0xc6: {
			uint8_t imme = fetch8();
			af.high = alu.add8(af.high, imme);
			break;
		}
		case 0x86: {
			uint8_t byte = addrBus.read(hl);
			af.high = alu.add8(af.high, byte);
			break;
		}
		case 0x88:
		case 0x89:
		case 0x8a:
		case 0x8b:
		case 0x8c:
		case 0x8d:
		case 0x8f: {
			uint8_t byte = getRegister(op & 0x7);
			af.high = alu.add8(af.high, byte, flags.getC());
			break;
		}
		case 0xce: {
			uint8_t imme = fetch8();
			af.high = alu.add8(af.high, imme, flags.getC());
			break;
		}
		case 0x8e: {
			uint8_t byte = addrBus.read(hl);
			af.high = alu.add8(af.high, byte, flags.getC());
			break;
		}
		case 0x90:
		case 0x91:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x97: {
			uint8_t byte = getRegister(op & 0x7);
			af.high = alu.sub8(af.high, byte);
			break;
		}
		case 0xd6: {
			uint8_t imme = fetch8();
			af.high = alu.sub8(af.high, imme);
			break;
		}
		case 0x96: {
			uint8_t byte = addrBus.read(hl);
			af.high = alu.sub8(af.high, byte);
			break;
		}
		case 0x98:
		case 0x99:
		case 0x9a:
		case 0x9b:
		case 0x9c:
		case 0x9d:
		case 0x9f: {
			uint8_t byte = getRegister(op & 0x7);
			af.high = alu.sub8(af.high, byte, 1 - flags.getC());
			break;
		}
		case 0xde: {
			uint8_t imme = fetch8();
			af.high = alu.sub8(af.high, imme, 1 - flags.getC());
			break;
		}
		case 0x9e: {
			uint8_t byte = addrBus.read(hl);
			af.high = alu.sub8(af.high, byte,1 - flags.getC());
			break;
		}
		case 0xa0:
		case 0xa1:
		case 0xa2:
		case 0xa3:
		case 0xa4:
		case 0xa5:
		case 0xa7: {
			uint8_t byte = getRegister(op & 0x7);
			af.high = alu.and8(af.high, byte);
			break;
		}
		case 0xe6: {
			uint8_t imme = fetch8();
			af.high = alu.and8(af.high, imme);
			break;
		}
		case 0xa6: {
			uint8_t byte = addrBus.read(hl);
			af.high = alu.and8(af.high, byte);
			break;
		}
		case 0xb0:
		case 0xb1:
		case 0xb2:
		case 0xb3:
		case 0xb4:
		case 0xb5:
		case 0xb7: {
			uint8_t byte = getRegister(op & 0x7);
			af.high = alu.or8(af.high, byte);
			break;
		}
		case 0xf6: {
			uint8_t imme = fetch8();
			af.high = alu.or8(af.high, imme);
			break;
		}
		case 0xb6: {
			uint8_t byte = addrBus.read(hl);
			af.high = alu.or8(af.high, byte);
			break;
		}
		case 0xa8:
		case 0xa9:
		case 0xaa:
		case 0xab:
		case 0xac:
		case 0xad:
		case 0xaf: {
			uint8_t byte = getRegister(op & 0x7);
			af.high = alu.xor8(af.high, byte);
			break;
		}
		case 0xee: {
			uint8_t imme = fetch8();
			af.high = alu.xor8(af.high, imme);
			break;
		}
		case 0xae: {
			uint8_t byte = addrBus.read(hl);
			af.high = alu.xor8(af.high, byte);
			break;
		}
		case 0xb8:
		case 0xb9:
		case 0xba:
		case 0xbb:
		case 0xbc:
		case 0xbd:
		case 0xbf: {
			uint8_t byte = getRegister(op & 0x7);
			alu.sub8(af.high, byte);
			break;
		}
		case 0xfe: {
			uint8_t imme = fetch8();
			alu.sub8(af.high, imme);
			break;
		}
		case 0xbe: {
			uint8_t byte = addrBus.read(hl);
			alu.sub8(af.high, byte);
			break;
		}
		case 0x4:
		case 0xc:
		case 0x14:
		case 0x1c:
		case 0x24:
		case 0x2c:
		case 0x3c: {
			uint8_t &dest = getRegister(op >> 3 & 0x7);
			dest = alu.inc8(dest);
			break;
		}
		case 0x34: {
			uint8_t byte = addrBus.read(hl);
			addrBus.write(hl, alu.inc8(byte));
			break;
		}
		case 0x5:
		case 0xd:
		case 0x15:
		case 0x1d:
		case 0x25:
		case 0x2d:
		case 0x3d: {
			uint8_t& dest = getRegister(op >> 3 & 0x7);
			dest = alu.dec8(dest);
			break;
		}
		case 0x35: {
			uint8_t byte = addrBus.read(hl);
			addrBus.write(hl, alu.dec8(byte));
			break;
		}


		case 0x9:
		case 0x19:
		case 0x29:
		case 0x39: {
			uint16_t value = getRegisterPair(op >> 4 & 0x3);
			int res = hl + value;
			flags.setN(false);
			flags.setC(res & 0x10000);
			flags.setH((hl & 0xfff) + (value & 0xfff) & 0x1000);
			break;
		}
		case 0xe8: {
			int8_t byte = fetch8();
			if (byte >= 0) {
				alu.add8(sp, byte);
 			} else {
				alu.sub8(sp, -byte);
			}
			sp = sp + byte;
			flags.setZ(false);
			flags.setN(false);
			break;
		}
		case 0x3:
		case 0x13:
		case 0x23:
		case 0x33: {
			gb_int &value = getRegisterPair(op >> 4 & 0x3);
			value = value + 1;
			break;
		}
		case 0xb:
		case 0x1b:
		case 0x2b:
		case 0x3b: {
			gb_int& value = getRegisterPair(op >> 4 & 0x3);
			value = value - 1;
			break;
		}


		case 0x7: {
			af.high = alu.rotLeft(af.high);
			break;
		}
		case 0x17: {
			af.high = alu.rotLeft(af.high, false);
			break;
		}
		case 0xf: {
			af.high = alu.rotRight(af.high);
			break;
		}
		case 0x1f: {
			af.high = alu.rotRight(af.high, false);
			break;
		}


		case 0xc3: {
			uint16_t addr = fetch16();
			pc = addr;
			break;
		}
		case 0xc2:
		case 0xca:
		case 0xd2:
		case 0xda: {
			uint16_t addr = fetch16();
			bool cond = getCondition(op >> 3 & 0x3);
			if (cond) {
				pc = addr;
			}
			break;
		}
		case 0x18: {
			int8_t imme = fetch8();
			pc = pc + imme;
			break;
		}
		case 0x20:
		case 0x28:
		case 0x30:
		case 0x38: {
			int8_t imme = fetch8();
			bool cond = getCondition(op >> 3 & 0x3);
			if (cond) {
				pc = pc + imme;
			}
			break;
		}
		case 0xe9: {
			pc = hl;
			break;
		}


		case 0xcd: {
			uint16_t addr = fetch16();
			addrBus.write(sp - 1, pc.high);
			addrBus.write(sp - 2, pc.low);
			pc = addr;
			sp = sp - 2;
			break;
		}
		case 0xc4:
		case 0xcc:
		case 0xd4:
		case 0xdc: {
			uint16_t addr = fetch16();
			bool cond = getCondition(op >> 3 & 0x3);
			if (cond) {
				addrBus.write(sp - 1, pc.high);
				addrBus.write(sp - 2, pc.low);
				pc = addr;
				sp = sp - 2;
			}
			break;
		}
		case 0xc9: {
			pc.low = addrBus.read(sp);
			pc.high = addrBus.read(sp + 1);
			sp = sp + 2;
			break;
		}
		case 0xd9: {
			pc.low = addrBus.read(sp);
			pc.high = addrBus.read(sp + 1);
			sp = sp + 2;
			ime = true;
			break;
		}
		case 0xc0:
		case 0xc8:
		case 0xd0:
		case 0xd8: {
			bool cond = getCondition(op >> 3 & 0x3);
			if (cond) {
				pc.low = addrBus.read(sp);
				pc.high = addrBus.read(sp + 1);
				sp = sp + 2;
			}
			break;
		}
		case 0xc7:
		case 0xcf:
		case 0xd7:
		case 0xdf:
		case 0xe7:
		case 0xef:
		case 0xf7:
		case 0xff: {
			uint16_t addr = ((op >> 3) & 0x3) << 3;
			addrBus.write(sp - 1, pc.high);
			addrBus.write(sp - 2, pc.low);
			pc = addr;
			sp = sp - 2;
			break;
		}


		case 0x27: {
			break;
		}
		case 0x2f: {
			af.high = ~af.high;
			flags.setH(true);
			flags.setN(true);
			break;
		}
		case 0x0: {
			break;
		}
		case 0x3f: {
			flags.setC(1 - flags.getC());
			flags.setH(false);
			flags.setN(false);
			break;
		}
		case 0x37: {
			flags.setC(true);
			flags.setH(false);
			flags.setN(false);
			break;
		}
		case 0xf3: {
			ime = false;
			break;
		}
		case 0xfb: {
			ime = true; // TODO delay this
			break;
		}
		case 0x76: {
			// TODO halt
			break;
		}
		case 0x10: {
			fetch8();
			// TODO stop
			break;
		}
		case 0xcb: {
			break;
		}
	}
}

void CPU::executeCB() {
	uint8_t op = fetch8();
	switch (op) {
		case 0x0:
		case 0x1:
		case 0x2:
		case 0x3:
		case 0x4:
		case 0x5:
		case 0x7: {
			uint8_t &byte = getRegister(op & 0x7);
			byte = alu.rotLeft(byte);
			flags.setZ(byte == 0);
			break;
		}
		case 0x6: {
			uint8_t byte = addrBus.read(hl);
			byte = alu.rotLeft(byte);
			addrBus.write(hl, byte);
			flags.setZ(byte == 0);
			break;
		}
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x17: {
			uint8_t& byte = getRegister(op & 0x7);
			byte = alu.rotLeft(byte, false);
			flags.setZ(byte == 0);
			break;
		}
		case 0x16: {
			uint8_t byte = addrBus.read(hl);
			byte = alu.rotLeft(byte, false);
			addrBus.write(hl, byte);
			flags.setZ(byte == 0);
			break;
		}
		case 0x8:
		case 0x9:
		case 0xa:
		case 0xb:
		case 0xc:
		case 0xd:
		case 0xf: {
			uint8_t& byte = getRegister(op & 0x7);
			byte = alu.rotRight(byte);
			flags.setZ(byte == 0);
			break;
		}
		case 0xe: {
			uint8_t byte = addrBus.read(hl);
			byte = alu.rotRight(byte);
			addrBus.write(hl, byte);
			flags.setZ(byte == 0);
			break;
		}
		case 0x18:
		case 0x19:
		case 0x1a:
		case 0x1b:
		case 0x1c:
		case 0x1d:
		case 0x1f: {
			uint8_t& byte = getRegister(op & 0x7);
			byte = alu.rotRight(byte, false);
			flags.setZ(byte == 0);
			break;
		}
		case 0x1e: {
			uint8_t byte = addrBus.read(hl);
			byte = alu.rotRight(byte, false);
			addrBus.write(hl, byte);
			flags.setZ(byte == 0);
			break;
		}
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x27: {
			uint8_t& byte = getRegister(op & 0x7);
			byte = alu.shiftLeft(byte);
			break;
		}
		case 0x26: {
			uint8_t byte = addrBus.read(hl);
			byte = alu.shiftLeft(byte);
			addrBus.write(hl, byte);
			break;
		}
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2c:
		case 0x2d:
		case 0x2f: {
			uint8_t& byte = getRegister(op & 0x7);
			byte = alu.shiftRight(byte);
			break;
		}
		case 0x2e: {
			uint8_t byte = addrBus.read(hl);
			byte = alu.shiftRight(byte);
			addrBus.write(hl, byte);
			break;
		}
		case 0x38:
		case 0x39:
		case 0x3a:
		case 0x3b:
		case 0x3c:
		case 0x3d:
		case 0x3f: {
			uint8_t& byte = getRegister(op & 0x7);
			byte = alu.shiftRight(byte, false);
			break;
		}
		case 0x3e: {
			uint8_t byte = addrBus.read(hl);
			byte = alu.shiftRight(byte, false);
			addrBus.write(hl, byte);
			break;
		}
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x37: {
			uint8_t& byte = getRegister(op & 0x7);
			byte = alu.swap(byte);
			break;
		}
		case 0x36: {
			uint8_t byte = addrBus.read(hl);
			byte = alu.swap(byte);
			addrBus.write(hl, byte);
			break;
		}
	}
}

uint8_t CPU::fetch8() {
	int8_t ret = addrBus.read(pc);
	pc = pc + 1;
	return ret;
}

uint16_t CPU::fetch16() {
	uint8_t low = fetch8();
	uint8_t high = fetch8();
	return (high << 8) | low;
}

uint8_t& CPU::getRegister(uint8_t reg) {
	switch (reg) {
		case 7:
			return af.high;
		case 0:
			return bc.high;
		case 1:
			return bc.low;
		case 2:
			return de.high;
		case 3:
			return de.low;
		case 4:
			return hl.high;
		case 5:
			return hl.low;
		default:
			throw std::exception();
	}
}

gb_int& CPU::getRegisterPair(uint8_t reg, bool isSP) {
	switch (reg) {
		case 0:
			return bc;
		case 1:
			return de;
		case 2:
			return hl;
		case 3:
			return isSP ? sp : af;
		default:
			throw std::exception();
	}
}

bool CPU::getCondition(uint8_t cc) {
	switch (cc) {
		case 0:
			return !flags.getZ();
		case 1:
			return flags.getZ();
		case 2:
			return !flags.getC();
		case 3:
			return flags.getC();
		default:
			throw std::exception();
	}
}