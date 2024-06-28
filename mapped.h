#pragma once

#include <cstdint>

class Mapped {
public:
    virtual void write(uint16_t addr, uint8_t byte) = 0;
    virtual void writeBit(uint16_t addr, uint8_t bit, bool on) = 0;
    virtual uint8_t read(uint16_t addr) const = 0;
    virtual bool readBit(uint16_t addr, uint8_t bit) const = 0;
};

class MultiAddrMapped : public Mapped {
public:
private:
};

class SingleAddrMapped : public Mapped {
public:
    virtual void write(uint8_t byte) = 0;
    virtual void writeBit(uint8_t bit, bool on) = 0;
    virtual uint8_t read() const = 0;
    virtual bool readBit(uint8_t bit) const = 0;
private:
    void write(uint16_t addr, uint8_t byte) override final {
        write(byte);
    }
    void writeBit(uint16_t addr, uint8_t bit, bool on) override final {
        writeBit(bit, on);
    }
    uint8_t read(uint16_t addr) const override final {
        return read();
    }
    bool readBit(uint16_t addr, uint8_t bit) const override final {
        return readBit(bit);
    }
};