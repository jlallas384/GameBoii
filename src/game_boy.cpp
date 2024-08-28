#include "game_boy.h"
#include <vector>
#include <cstdint>
#include <fstream>
#include <utility>
#include <exception>
#include "cartridge.h"

GameBoy::GameBoy(std::unique_ptr<LCD> lcd) : cpu(addrBus), irqHandler(cpu, addrBus), joypad(addrBus, irqHandler), ppu(addrBus, std::move(lcd), irqHandler), apu(addrBus), timers(addrBus, irqHandler, apu) {
    std::ifstream file("cgb_boot.bin", std::ios::binary);
    file.read(reinterpret_cast<char*>(bootROM.data()), bootROM.size());
    for (int i = 0; i < 2304; i++) {
        addrBus.setReader(i, bootROM[i]);
    }


    addrBus.setWriter(0xff50, [&](uint8_t byte) {
        if (byte && cartridge && bootROMEnabled) {
            bootROMEnabled = false;
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

    addrBus.setReader(0xff4d, [&]() {
        return (cpu.isDoubleSpeed() << 7) | (key1 & 1);
    });
    addrBus.setWriter(0xff4d, [&](uint8_t byte) {
        if ((key1 & 1) != (byte & 1)) {
            key1 ^= 1;
        }
    });
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

std::unique_ptr<Cartridge> createCartridge(uint8_t value, const std::vector<uint8_t>& rom, uint32_t ramSize, const std::filesystem::path& path) {
    MapperKind kind;
    bool hasBattery = false;
    switch (value) {
        case 0x0:
            kind = MapperKind::kNone;
            break;
        case 0x1:
        case 0x2:
        case 0x3:
            kind = MapperKind::kMBC1;
            hasBattery = value == 0x3;
            break;
        case 0x5:
        case 0x6:
            kind = MapperKind::kMBC2;
            hasBattery = value == 0x6;
            break;
        case 0xf:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            kind = MapperKind::kMBC3;
            hasBattery = value != 0x11 && value != 0x12;
            break;
        case 0x1a:
        case 0x1b:
            kind = MapperKind::kMBC5;
            hasBattery = value == 0x1b;
            break;
        default:
            throw std::runtime_error("mapper is not supported");
    }
    return Cartridge::create(kind, rom, ramSize, path, hasBattery);
}

void GameBoy::loadCartridge(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    file.seekg(0x148);
    auto value = file.peek(); // TODO error check
    uint32_t romSize = 32768 * (1 << value);
    std::vector<uint8_t> rom(romSize);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(rom.data()), romSize);
    uint32_t ramSize = getRAMSize(rom[0x149]);

    cartridge = createCartridge(rom[0x147], rom, ramSize, path);

    cartridge->loadRAM();
    cartridge->loadToAddrBus(addrBus);


    for (int i = 0; i <= 0xff; i++) {
        addrBus.setReader(i, bootROM[i]);
    }
    for (int i = 0x200; i <= 0x8ff; i++) {
        addrBus.setReader(i, bootROM[i]);
    }

    bootROMEnabled = true;
    ppu.reset();
    cpu.reset();
    apu.reset();
    key1 = 0;
}

void GameBoy::saveState() {
    if (!bootROMEnabled) {
        pendingSave = true;
    }
}

void GameBoy::loadState() {
    if (!bootROMEnabled) {
        pendingLoad = true;
    }
}

void GameBoy::run() {
    handlePending();
    cpu.tick();
    if (cpu.isDoubleSpeed()) {
        cpu.tick();
    }
    const bool isDoubleSpeed = cpu.isDoubleSpeed();
    ppu.tick(isDoubleSpeed);
    ppu.tick(isDoubleSpeed);
    ppu.tick(isDoubleSpeed);
    ppu.tick(isDoubleSpeed);
    timers.tick(isDoubleSpeed);
    timers.tick(isDoubleSpeed);
    timers.tick(isDoubleSpeed);
    timers.tick(isDoubleSpeed);
    if (isDoubleSpeed) {
        timers.tick(isDoubleSpeed);
        timers.tick(isDoubleSpeed);
        timers.tick(isDoubleSpeed);
        timers.tick(isDoubleSpeed);
    }
    apu.tick();
    apu.tick();
    apu.tick();
    apu.tick();
    irqHandler.handle();
}

void GameBoy::serialize(std::ofstream& of) const {
    using ::serialize;
    cpu.serialize(of);
    irqHandler.serialize(of);
    joypad.serialize(of);
    ppu.serialize(of);
    timers.serialize(of);
    cartridge->serialize(of);
    serialize(of, hram);
    serialize(of, wramBank);
    serialize(of, key1);
    serialize(of, wram);
}

void GameBoy::deserialize(std::ifstream& is) {
    using ::deserialize;
    cpu.deserialize(is);
    irqHandler.deserialize(is);
    joypad.deserialize(is);
    ppu.deserialize(is);
    timers.deserialize(is);
    cartridge->deserialize(is);
    deserialize(is, hram);
    deserialize(is, wramBank);
    deserialize(is, key1);
    deserialize(is, wram);
}

void GameBoy::handlePending() {
    if ((addrBus.read(0xff41) & 3) == 1) {
        if (pendingLoad) {
            pendingLoad = false;
            if (auto path = cartridge->getPath().replace_extension("ss"); exists(path)) {
                std::ifstream is(path, std::ios::binary);
                deserialize(is);
            }
        }

        if (pendingSave) {
            pendingSave = false;
            auto path = cartridge->getPath().replace_extension("ss");
            std::ofstream of(path, std::ios::binary);
            serialize(of);
        }
    }
}
