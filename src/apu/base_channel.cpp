#include "base_channel.h"
#include "utils.h"

float BaseChannel::getAnalogOutput() const {
    return !dacEnable ? 0.0 : 1.0 - digitalOutput / 15.0 * 2;
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

void BaseChannel::tickVolume() {
    if (uint8_t pace = reg2 & 0x3; pace) {
        volumeTicksLeft--;
        if (!volumeTicksLeft) {
            bool direction = getBit(reg2, 3);
            if (!direction && volume) {
                volume--;
            } else if (direction && volume < 0xf) {
                volume++;
            }
            volumeTicksLeft = pace;
        }
    }
}

void BaseChannel::tick() {
    if (!active) {
        digitalOutput = 0;
        return;
    }
    tickImpl();
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
