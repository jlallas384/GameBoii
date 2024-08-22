#pragma once

#include "base_channel.h"

class AddressBus;

class Channel1 : public BaseChannel {
public:
    Channel1(AddressBus& addrBus);
    void tickSweep();
private:
    void tickImpl() override;
    static constexpr uint8_t waveforms[] = { 0b10000000, 0b10000001, 0b11100001, 0b01111110 };
    uint8_t dividerTicks = 0, waveformIndex = 0, ticksLeft = 0;
    uint16_t periodTicks = 0;
};