#pragma once
#include <functional>
#include <vector>
#include <cstdint>

class AddressBus {
public:
    using Reader = std::function<uint8_t()>;
    using Writer = std::function<void(uint8_t)>;

    AddressBus();
    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t byte) const;
    bool readBit(uint16_t addr, uint8_t bit) const;
    void writeBit(uint16_t addr, uint8_t bit, bool on) const;
    void setReader(uint16_t addr, const Reader& reader);
    void setReader(uint16_t addr, const uint8_t& val);
    void setWriter(uint16_t addr, const Writer& writer);
    void setWriter(uint16_t addr, uint8_t& val);
private:
    std::vector<Reader> readAddrs;
    std::vector<Writer> writeAddrs;
};