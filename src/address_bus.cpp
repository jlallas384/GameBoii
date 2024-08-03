#include "address_bus.h"
#include <algorithm>
#include "utils.h"

AddressBus::AddressBus() : readAddrs(1 << 16), writeAddrs(1 << 16){
    std::fill(readAddrs.begin(), readAddrs.end(), [&]() {
        return 0xff;
    });
    std::fill(writeAddrs.begin(), writeAddrs.end(), [&](uint8_t byte) {});
}

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
