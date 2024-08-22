#pragma once

#include <cstdint>
#include "channel1.h"
#include "channel2.h"
#include "channel3.h"
#include "channel4.h"

class AddressBus;

class APU {
public:
    APU(AddressBus& addrBus);
    void tick();
    void divTick();
private:
    void sample() const;
    Channel1 ch1;
    Channel2 ch2;
    Channel3 ch3;
    Channel4 ch4;
    uint8_t masterControl = 0, masterVolume = 0, panning = 0;
    uint8_t divTickCount = 0;
};