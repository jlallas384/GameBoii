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
    bool setButtonState(uint8_t state);
    bool setDPadState(uint8_t state);
private:
    uint8_t read() const override;
    uint8_t buttonState = 0xf, dPadState = 0xf;
};