#pragma once
#include <memory>
#include <filesystem>
#include <array>
#include <fstream>
#include "cartridge.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "address_bus.h"
#include "irq_handler.h"
#include "ppu/lcd.h"
#include "timers.h"
#include "joypad.h"
#include "apu/apu.h"

class GameBoy {
public:
    GameBoy(std::unique_ptr<LCD> lcd);
    Joypad& getJoypad();
    void loadCartridge(const std::filesystem::path& path);
    void saveState();
    void loadState();
    void run();
private:
    void serialize(std::ofstream& of) const;
    void deserialize(std::ifstream& is);
    void handlePending();
    AddressBus addrBus;
    CPU cpu;
    IRQHandler irqHandler;
    Joypad joypad;
    PPU ppu;
    APU apu;
    Timers timers;

    std::unique_ptr<Cartridge> cartridge;
    std::array<uint8_t, 127> hram{};
    uint8_t wramBank = 1, key1 = 0;
    std::array<std::array<uint8_t, 4096>, 8> wram{};
    std::array<uint8_t, 2304> bootROM;
    bool pendingSave = false, pendingLoad = false, bootROMEnabled = true;
};