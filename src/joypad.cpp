#include "joypad.h"
#include "irq_handler.h"
#include "utils.h"
#include "address_bus.h"

Joypad::Joypad(AddressBus& addrBus, IRQHandler& irqHandler) : irqHandler(irqHandler) {
    addrBus.setReader(0xff00, [&]() {
        return !getBit(joypadRegister, 5) ? joypadRegister | buttonsState : joypadRegister | dPadState;
    });

    addrBus.setWriter(0xff00, [&](uint8_t byte) {
        joypadRegister = byte & 0xf0;
    });
}

void Joypad::press(DPadKind kind) {
    dPadKindImpl(kind, true);
}

void Joypad::unpress(DPadKind kind) {
    dPadKindImpl(kind, false);
}

void Joypad::press(ButtonKind kind) {
    buttonKindImpl(kind, true);
}

void Joypad::unpress(ButtonKind kind) {
    buttonKindImpl(kind, false);
}

void Joypad::serialize(std::ofstream& of) const {
    using ::serialize;
    serialize(of, joypadRegister);
}

void Joypad::dPadKindImpl(DPadKind kind, bool on) {
    if (!getBit(joypadRegister, 4) && getBit(dPadState, static_cast<uint8_t>(kind)) && on) {
        irqHandler.request(IRQHandler::kJoypad);
    }
    dPadState = setBit(dPadState, static_cast<uint8_t>(kind), !on);
}

void Joypad::buttonKindImpl(ButtonKind kind, bool on) {
    if (!getBit(joypadRegister, 5) && getBit(buttonsState, static_cast<uint8_t>(kind)) && on) {
        irqHandler.request(IRQHandler::kJoypad);
    }
    buttonsState = setBit(buttonsState, static_cast<uint8_t>(kind), !on);
}
