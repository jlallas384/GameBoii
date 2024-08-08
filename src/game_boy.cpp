#include "game_boy.h"
#include <vector>
#include <cstdint>
#include <fstream>
#include <utility>
#include <exception>
#include "cartridge.h"
#include <iostream>

GameBoy::GameBoy(std::unique_ptr<LCD> lcd) : cpu(addrBus), irqHandler(cpu, addrBus), joypad(addrBus, irqHandler), ppu(addrBus, std::move(lcd), irqHandler), timers(addrBus, irqHandler) {
    std::ifstream file("cgb_boot.bin", std::ios::binary);
    file.read(reinterpret_cast<char*>(bootROM.data()), bootROM.size());
    for (int i = 0; i < 2304; i++) {
        addrBus.setReader(i, bootROM[i]);
    }

    for (int i = 0xff10; i <= 0xff26; i++) {
        addrBus.setReader(i, [&]() {
            return 0;
        });
        addrBus.setWriter(i, [&](uint8_t byte) {

        });
    }
    for (int i = 0xff30; i <= 0xff3f; i++) {
        addrBus.setReader(i, [&]() {
            return 0;
        });
        addrBus.setWriter(i, [&](uint8_t byte) {

        });
    }

    addrBus.setWriter(0xff50, [&](uint8_t byte) {
        if (byte && cartridge) {
            cartridge->loadToAddrBus(addrBus);
        }
    });

    for (int i = 0; i < hram.size(); i++) {
        addrBus.setReader(0xff80 + i, hram[i]);
        addrBus.setWriter(0xff80 + i, hram[i]);
    }

    addrBus.setReader(0xff70, wramBank);
    addrBus.setWriter(0xff70, [&](uint8_t byte) {
        wramBank = byte & 0x7;
        if (!wramBank) wramBank = 1;
    });

    for (int i = 0; i < 4096; i++) {
        addrBus.setReader(0xc000 + i, wram[0][i]);
        addrBus.setWriter(0xc000 + i, wram[0][i]);

        addrBus.setReader(0xd000 + i, [&, i]() {
            return wram[wramBank][i];
        });
        addrBus.setWriter(0xd000 + i, [&, i](uint8_t byte) {
            wram[wramBank][i] = byte;
        });
    }
}

Joypad& GameBoy::getJoypad() {
    return joypad;
}

uint32_t getRAMSize(uint8_t value) {
    switch (value) {
        case 0x0:
        case 0x1:
            return 0;
        case 0x2:
            return 8192;
        case 0x3:
            return 32768;
        case 0x4:
            return 131072;
        case 0x5:
            return 65536;
        default:
            return 0;
            // todo error;
    }
}

std::pair<std::unique_ptr<Cartridge>, bool> createCartidge(uint8_t value, std::vector<uint8_t>& rom, uint32_t ramSize) {
    std::unique_ptr<Cartridge> cartridge;
    bool hasBattery = false;
    switch (value) {
        case 0x0:
            cartridge = Cartridge::create(MapperKind::kNone, rom, ramSize);
            break;
        case 0x1:
        case 0x2:
        case 0x3:
            cartridge = Cartridge::create(MapperKind::kMBC1, rom, ramSize);
            hasBattery = value == 0x3;
            break;
        case 0x5:
        case 0x6:
            cartridge = Cartridge::create(MapperKind::kMBC2, rom, ramSize);
            hasBattery = value == 0x6;
            break;
        case 0xf:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            cartridge = Cartridge::create(MapperKind::kMBC3, rom, ramSize);
            hasBattery = value != 0x11 && value != 0x12;
            break;
        default:
            throw std::runtime_error("mapper is not supported");
    }
    return std::make_pair(std::move(cartridge), hasBattery);
}

void GameBoy::loadCartridge(std::filesystem::path path) {
    std::ifstream file(path, std::ios::binary);
    file.seekg(0x148);
    auto value = file.peek(); // TODO error check
    uint32_t romSize = 32768 * (1 << value);
    std::vector<uint8_t> rom(romSize);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(rom.data()), romSize);
    uint32_t ramSize = getRAMSize(rom[0x149]);

    bool hasBattery;
    std::tie(cartridge, hasBattery) = createCartidge(rom[0x147], rom, ramSize);

    if (hasBattery) {
        cartridge->loadRAM(path.replace_extension("sav"));
    }

    cartridge->loadToAddrBus(addrBus);

    for (int i = 0; i <= 0xff; i++) {
        addrBus.setReader(i, bootROM[i]);
    }
    for (int i = 0x200; i <= 0x8ff; i++) {
        addrBus.setReader(i, bootROM[i]);
    }
    ppu.reset();
    cpu.reset();
}

void GameBoy::run() {
    cpu.tick();
    ppu.tick();
    ppu.tick();
    ppu.tick();
    ppu.tick();
    timers.tick();
    timers.tick();
    timers.tick();
    timers.tick();
    irqHandler.handle();
}