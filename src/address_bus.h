#pragma once
#include <functional>
#include <vector>

class AddressBus {
public:
    using Reader = std::function<uint8_t()>;
    using Writer = std::function<void(uint8_t)>;

    AddressBus();
    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t byte);
    bool readBit(uint16_t addr, uint8_t bit) const;
    void writeBit(uint16_t addr, uint8_t bit, bool on);
    void setReader(uint16_t addr, Reader reader);
    void setReader(uint16_t addr, const uint8_t& val);
    void setWriter(uint16_t addr, Writer writer);
    void setWriter(uint16_t addr, uint8_t& val);
private:
    std::vector<Reader> readAddrs;
    std::vector<Writer> writeAddrs;
};