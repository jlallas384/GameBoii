#include "cartridge.h"

Cartridge::Cartridge(std::ifstream &file) : rom(loadROM(file)), ram(ramSize()), mapper(createMapper()) {

}

void Cartridge::loadToAddrBus(AddressBus& addrBus) {
    // TODO check mapper is not null
    mapper->loadToAddrBus(addrBus);
}

std::vector<uint8_t> Cartridge::loadROM(std::ifstream& file) {
    file.seekg(0x148);
    auto value = file.peek(); // TODO error check
    auto size = 32768 * (1 << value);
    std::vector<uint8_t> ret(size);
    file.read(reinterpret_cast<char*>(ret.data()), size);
    return ret;
}

uint32_t Cartridge::ramSize() const {
    switch (rom[0x149]) {
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

std::unique_ptr<Mapper> Cartridge::createMapper() {
    switch (rom[0x147]) {
        case 0x0:
            return Mapper::create(0, rom, ram);
        case 0x1:
        case 0x2:
        case 0x3:
            return Mapper::create(1, rom, ram);
        case 0x4:
        case 0x5:
            return Mapper::create(2, rom, ram);
        case 0x11:
        case 0x12:
        case 0x13:
            return Mapper::create(3, rom, ram);
    }
}
