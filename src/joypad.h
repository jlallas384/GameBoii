#pragma once
#include <cstdint>

enum class DPadKind {
    kRight,
    kLeft,
    kUp,
    kDown
};

enum class ButtonKind {
    kA,
    kB,
    kSelect,
    kStart,
};

class IRQHandler;
class AddressBus;

class Joypad {
public:
    Joypad(AddressBus& addrBus, IRQHandler& irqHandler);
    void press(DPadKind kind);
    void unpress(DPadKind kind);
    void press(ButtonKind kind);
    void unpress(ButtonKind kind);
private:
    void dPadKindImpl(DPadKind kind, bool on);
    void buttonKindImpl(ButtonKind kind, bool on);
    IRQHandler& irqHandler;
    uint8_t joypadRegister = 0xff, buttonsState = 0xff, dPadState = 0xff;
};