#include "ioregs.h"
#include "utils.h"

bool JoypadReg::setButtonState(uint8_t state) {
    bool ret = (buttonState & state) && !getBit(value, 5);
    buttonState = ~state & 0xf;
    return ret;
}

bool JoypadReg::setDPadState(uint8_t state) {
    bool ret = (dPadState & state) && !getBit(value, 4);
    dPadState = ~state & 0xf;
    return ret;
}

uint8_t JoypadReg::read() const {
    if (!getBit(value, 5)) {
        return (value & 0xf0) | buttonState;
    }
    if (!getBit(value, 4)) {
        return (value & 0xf0) | dPadState;
    }
    return 0xf;
}
