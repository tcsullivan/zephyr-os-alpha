#include "digital.hpp"

#include <ch.h>
#include <hal.h>

// a lookup table for pads and ports
static const int portPad[12][2] = {
	{GPIOE_BASE,  9}, // digital pin 1
	{GPIOE_BASE, 11}, // 2
	{GPIOC_BASE,  6}, // ...
	{GPIOC_BASE,  7},
	{GPIOE_BASE, 13},
	{GPIOE_BASE, 14},
	{GPIOE_BASE,  8},
	{GPIOE_BASE, 10},
	{GPIOE_BASE, 12},
	{GPIOE_BASE,  7},
	{GPIOD_BASE,  0},
	{GPIOD_BASE,  1},
};

namespace digital {
	void setMode(int pin, int output) {
		const auto& pp = portPad[pin - 1];
		palSetPadMode(reinterpret_cast<GPIO_TypeDef*>(pp[0]), pp[1],
		              output ? PAL_MODE_OUTPUT_PUSHPULL : PAL_MODE_INPUT_PULLUP);
	}

	void set(int pin, int val) {
		const auto& pp = portPad[pin - 1];
		if (val)
			palSetPad(reinterpret_cast<GPIO_TypeDef*>(pp[0]), pp[1]);
		else
			palClearPad(reinterpret_cast<GPIO_TypeDef*>(pp[0]), pp[1]);
	}
}

