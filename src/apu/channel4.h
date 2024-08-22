#pragma once

#include "base_channel.h"

class AddressBus;

class Channel4 : public BaseChannel {
public:
    Channel4(AddressBus& addrBus);
private:
    void tickImpl() override;
    bool shiftLFSR();
    static constexpr uint8_t divisors[] = { 8, 16, 32, 48, 64, 80, 96, 112 };
    uint16_t lfsr = 0;
    uint32_t clockTicksLeft = 0;
};