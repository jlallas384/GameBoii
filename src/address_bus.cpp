#include "address_bus.h"

#include "utils.h"

uint8_t AddressBus::read(uint16_t addr) const {
    return readAddrs[addr]();
}

void AddressBus::write(uint16_t addr, uint8_t byte) {
    writeAddrs[addr](byte);
}

bool AddressBus::readBit(uint16_t addr, uint8_t bit) const {
    return getBit(read(addr), bit);
}

void AddressBus::writeBit(uint16_t addr, uint8_t bit, bool on) {
    return write(addr, setBit(read(addr), bit, on));
}

void AddressBus::setReader(uint16_t addr, Reader reader) {
    readAddrs[addr] = reader;
}

void AddressBus::setReader(uint16_t addr, const uint8_t& val) {
    readAddrs[addr] = [&]() {
        return val;
    };
}

void AddressBus::setWriter(uint16_t addr, Writer writer) {
    writeAddrs[addr] = writer;
}

void AddressBus::setWriter(uint16_t addr, uint8_t& val) {
    writeAddrs[addr] = [&](uint8_t byte) {
        val = byte;
    };
}
