#pragma once
#include <memory>
#include <filesystem>
#include <array>
#include "cartridge.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "address_bus.h"
#include "irq_handler.h"
#include "ppu/lcd.h"
#include "timers.h"

class GameBoy {
public:
    enum Button {
        kA = 0,
        kB = 1,
        kSelect = 2,
        kStart = 3,
        kRight = 0,
        kLeft = 1,
        kUp = 2,
        kDown = 3
    };
    GameBoy(std::unique_ptr<LCD> lcd);
    void loadCartridge(std::filesystem::path path);
    void run();
    void changeButtonState(Button button, bool on);
    void changeDPadState(Button button, bool on);
private:
    AddressBus addrBus;
    CPU cpu;
    IRQHandler irqHandler;
    PPU ppu;
    Timers timers;
    uint8_t joypadRegister = 0xff, buttonsState = 0xff, dPadState = 0xff;
    std::unique_ptr<Cartridge> cartridge;
    std::array<uint8_t, 127> hram{};
    std::array<uint8_t, 8192> wram{};
};