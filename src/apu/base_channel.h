#pragma once

#include <cstdint>

class BaseChannel {
public:
    float getAnalogOutput() const;
    void tickLength();
    void tickVolume();
    void tick();
    bool isActive() const;
private:
    virtual void tickImpl() = 0;
protected:
    void setDacEnable(bool on);
    bool active = false, dacEnable = false;
    uint16_t lengthTimer = 0;
    uint8_t volume = 0, volumeTicksLeft = 0;
    uint8_t digitalOutput = 0;
    uint8_t reg0 = 0, reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0;
};