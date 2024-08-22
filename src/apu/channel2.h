#pragma once

#include "base_channel.h"

class AddressBus;

class Channel2 : public BaseChannel {
public:
    Channel2(AddressBus& addrBus);
private:
    void tickImpl() override;
    static constexpr uint8_t waveforms[] = { 0b10000000, 0b10000001, 0b11100001, 0b01111110 };
    uint8_t dividerTicks = 0, waveformIndex = 0;
    uint16_t periodTicks = 0;
};