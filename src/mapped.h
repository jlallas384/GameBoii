#pragma once

#include <cstdint>
#include <array>
#include <algorithm>
#include "utils.h"

class Mapped {
public:
    virtual ~Mapped() {}
    virtual void write(uint16_t addr, uint8_t byte) = 0;
    virtual uint8_t read(uint16_t addr) const = 0;
    bool readBit(uint16_t addr, uint8_t bit) const {
        return read(addr) & (1 << bit);
    }
    void writeBit(uint16_t addr, uint8_t bit, bool on) {
        write(addr, setBit(read(addr), bit, on));
    }
};

class MultiAddrMapped : public Mapped {
public:
    void write(uint16_t addr, uint8_t byte) override {
        addrs[addr]->write(addr, byte);
    }
    uint8_t read(uint16_t addr) const override {
        return addrs[addr]->read(addr);
    }
    void setRange(uint16_t left, uint16_t right, Mapped* mapped) {
        std::fill(addrs.begin() + left, addrs.begin() + right + 1, mapped);
    }
private:
    std::array<Mapped*, 1 << 16> addrs = {};
};

class SingleAddrMapped : public Mapped {
public:
    SingleAddrMapped(uint8_t value = 0) : value(value) {}
    virtual void write(uint8_t byte) {
        value = byte;
    }
    virtual uint8_t read() const {
        return value;
    }
private:
    void write(uint16_t addr, uint8_t byte) override final {
        write(byte);
    }
    uint8_t read(uint16_t addr) const override final {
        return read();
    }
protected:
    uint8_t value;
};