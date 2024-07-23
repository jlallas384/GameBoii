#pragma once
#include <functional>
#include <array>

#include "utils.h"
class AddressBus {
public:
    using Reader = std::function<uint8_t()>;
    using Writer = std::function<void(uint8_t)>;
    uint8_t read(uint16_t addr) const {
        return readAddrs[addr]();
    }
    virtual void write(uint16_t addr, uint8_t byte) {
        writeAddrs[addr](byte);
    }
    bool readBit(uint16_t addr, uint8_t bit) {
        return getBit(read(addr), bit);
    }
    void writeBit(uint16_t addr, uint8_t bit, bool on) {
        return write(addr, setBit(read(addr), bit, on));
    }
    void setReader(uint16_t addr, Reader reader) {
        readAddrs[addr] = reader;
    }
    void setWriter(uint16_t addr, Writer writer) {
        writeAddrs[addr] = writer;
    }
private:
    std::array<Reader, 1 << 16> readAddrs;
    std::array<Writer, 1 << 16> writeAddrs;
};