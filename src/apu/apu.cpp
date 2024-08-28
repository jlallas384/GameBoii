#include "apu.h"
#include "address_bus.h"
#include "utils.h"
#include "audio_engine.h"

APU::APU(AddressBus& addrBus) : ch1(addrBus), ch2(addrBus), ch3(addrBus), ch4(addrBus) {
    addrBus.setReader(0xff24, masterVolume);
    addrBus.setWriter(0xff24, masterVolume);
    addrBus.setReader(0xff25, panning);
    addrBus.setWriter(0xff25, panning);
    addrBus.setReader(0xff26, [&]() {
        return (masterControl & (1 << 7)) | (ch4.isActive() << 3) | (ch3.isActive() << 2) | (ch2.isActive() << 1) | ch1.isActive();
    });
    addrBus.setWriter(0xff26, [&](uint8_t byte) {
        if (getBit(masterControl, 7) != getBit(byte, 7)) {
            
        }
        masterControl = byte;
    });
}

void APU::tick() {
    static int ticks = 0;
    if (getBit(masterControl, 7)) {
        ch1.tick();
        ch2.tick();
        ch3.tick();
        ch4.tick();
    }
    if (ticks % (1 << 6) == 0) {
        sample();
    }
    ticks++;
    if (ticks == (1 << 22)) ticks = 0;
}

void APU::divTick() {
    divTickCount++;
    if (divTickCount % 2 == 0) {
        ch1.tickLength();
        ch2.tickLength();
        ch3.tickLength();
        ch4.tickLength();
    }
    if (divTickCount % 4 == 0) {
        ch1.tickSweep();
    }
    if (divTickCount % 8 == 0) {
        ch1.tickVolume();
        ch2.tickVolume();
        ch4.tickVolume();
        divTickCount = 0;
    }
}

void APU::reset() {
    masterControl = 0;
    masterVolume = 0;
    panning = 0;
}

float applyVolume(float input, uint8_t volume) {
    return input * (1.0 / 8.0) * volume;
}

void APU::sample() const {
    float left = 0, right = 0;
    float ch1Output = ch1.getAnalogOutput();
    float ch2Output = ch2.getAnalogOutput();
    float ch3Output = ch3.getAnalogOutput();
    float ch4Output = ch4.getAnalogOutput();

    if (getBit(panning, 4)) {
        left += ch1Output;
    }
    if (getBit(panning, 0)) {
        right += ch1Output;
    }

    if (getBit(panning, 5)) {
        left += ch2Output;
    }
    if (getBit(panning, 1)) {
        right += ch2Output;
    }

    if (getBit(panning, 6)) {
        left += ch3Output;
    }
    if (getBit(panning, 2)) {
        right += ch3Output;
    }

    if (getBit(panning, 7)) {
        left += ch4Output;
    }
    if (getBit(panning, 3)) {
        right += ch4Output;
    }

    uint8_t leftVolume = ((masterVolume >> 4) & 0x7) + 1;
    uint8_t rightVolume = (masterVolume & 0x7) + 1;
    AudioEngine& engine = AudioEngine::getInstance();
    float leftOutput = applyVolume(left, leftVolume);
    float rightOutput = applyVolume(right, rightVolume);
    engine.pushSample(leftOutput, rightOutput);
}
