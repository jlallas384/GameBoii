#pragma once

#include <cstdint>

class Flags {
public:
	Flags(uint8_t& value) : value(value) {}
	void setZ(bool b) {
		value = (value & ~0x80) | (b << 7);
	}
	void setN(bool b) {
		value = (value & ~0x40) | (b << 6);
	}
	void setH(bool b) {
		value = (value & ~0x20) | (b << 5);
	}
	void setC(bool b) {
		value = (value & ~0x10) | (b << 4);
	}
	bool getZ() const {
		return value & 0x80;
	}
	bool getC() const {
		return value & 0x10;
	}
	bool getH() const {
		return value & 0x20;
	}
private:
	uint8_t& value;
};