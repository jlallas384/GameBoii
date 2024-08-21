#pragma once

#include <cstdint>

class BaseChannel {
public:
    float getAnalogOutput() const;
    void tickLength();
    bool isActive() const;
    static constexpr uint8_t muted = 16;
private:
    bool dacEnable = false;
    uint8_t lengthTimer = 0;
protected:
    void setDacEnable(bool on);
    bool active = false;
    uint8_t digitalOutput = 0;
    uint8_t reg0 = 0, reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0;
};