#pragma once
#include <memory>
#include <filesystem>
#include "cartridge.h"
#include "cpu/cpu.h"
#include "address_bus.h"

class GameBoy {
public:
    GameBoy();
    void loadCartridge(std::filesystem::path path);
private:
    AddressBus addrBus;
    CPU cpu;
    std::unique_ptr<Cartridge> cartridge;
};