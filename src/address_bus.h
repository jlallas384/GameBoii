#pragma once
#include <functional>
#include <array>

class AddressBus {
public:
    using Reader = std::function<uint8_t()>;
    using Writer = std::function<void(uint8_t)>;

    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t byte);
    bool readBit(uint16_t addr, uint8_t bit) const;
    void writeBit(uint16_t addr, uint8_t bit, bool on);
    void setReader(uint16_t addr, Reader reader);
    void setReader(uint16_t addr, const uint8_t& val);
    void setWriter(uint16_t addr, Writer writer);
    void setWriter(uint16_t addr, uint8_t& val);
private:
    std::array<Reader, 1 << 16> readAddrs;
    std::array<Writer, 1 << 16> writeAddrs;
};