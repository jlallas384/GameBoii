#include "base_channel.h"
#include "utils.h"

float BaseChannel::getAnalogOutput() const {
    return digitalOutput == muted || !dacEnable ? 0.0 : 1.0 - 1.0 / 15.0 * digitalOutput;
}

void BaseChannel::tickLength() {
    if (getBit(reg4, 6)) {
        if (lengthTimer) {
            lengthTimer--;
        }
        if (!lengthTimer) {
            active = false;
        }
    }
}

bool BaseChannel::isActive() const {
    return active;
}

void BaseChannel::setDacEnable(bool on) {
    dacEnable = on;
    if (!dacEnable) {
        active = false;
    }
}
