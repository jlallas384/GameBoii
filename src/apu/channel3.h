#pragma once

#include "base_channel.h"
#include <array>

class AddressBus;

class Channel3 : public BaseChannel {
public:
    Channel3(AddressBus& addrBus);
private:
    void tickImpl() override;
    uint8_t sampleAt(uint8_t index) const;
    std::array<uint8_t, 16> waveRAM{};
    uint8_t dividerTicks = 0;
    uint8_t sampleIndex = 0;
    uint16_t periodTicks = 0;
};