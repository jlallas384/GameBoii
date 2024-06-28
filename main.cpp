#include <cstdint>
#include <iostream>
#include "cpu.h"
#include "addressbus.h"
#include "ioregs.h"


int main() {
	Mapped* m = new JoypadReg;
	m->write(100, 0b00011111);
	JoypadReg* p = dynamic_cast<JoypadReg*>(m);
	std::cout << p->setButtonState(p->kStart);
	std::cout << p->setButtonState(p->kStart);
	std::cout << p->setButtonState(0);
	std::cout << p->setButtonState(p->kStart);
	std::cout << p->setButtonState(p->kStart);
}
