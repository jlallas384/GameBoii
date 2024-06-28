#pragma once

#include "mapped.h"

class JoypadReg : public SingleAddrMapped {
public:
    enum Buttons : uint8_t {
        kStart = 0x8,
        kSelect = 0x4,
        kB = 0x2,
        kA = 0x1,
        kDown = 0x8,
        kUp = 0x4,
        kLeft = 0x2,
        kRight = 0x1
    };
    bool setButtonState(uint8_t state) {
        bool ret = (buttonState & state) && !(value & 0x20);
        buttonState = ~state & 0xf;
        return ret;
    }
    bool setDPadState(uint8_t state) {
        bool ret = (dPadState & state) && !(value & 0x10);
        dPadState = ~state & 0xf;
        return ret;
    }
private:
    void write(uint8_t byte) override {
        value = byte & 0xf0;
    }
    uint8_t read() const override {
        if (!(value & 0x20)) {
            return value | buttonState;
        }
        if (!(value & 0x10)) {
            return value | dPadState;
        }
        return 0xf;
    }
    uint8_t value = 0xff, buttonState = 0xf, dPadState = 0xf;
};