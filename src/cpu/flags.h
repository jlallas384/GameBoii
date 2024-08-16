#pragma once

#include <cstdint>
#include "utils.h"

class Flags {
public:
    Flags(uint8_t& value) : value(value) {}
    void setZ(bool b) const {
        value = setBit(value, 7, b);
    }
    void setN(bool b) const {
        value = setBit(value, 6, b);
    }
    void setH(bool b) const {
        value = setBit(value, 5, b);
    }
    void setC(bool b) const {
        value = setBit(value, 4, b);
    }
    bool getZ() const {
        return getBit(value, 7);
    }
    bool getN() const {
        return getBit(value, 6);
    }
    bool getH() const {
        return getBit(value, 5);
    }
    bool getC() const {
        return getBit(value, 4);
    }
private:
    uint8_t& value;
};