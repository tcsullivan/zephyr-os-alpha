#include <ch.h>
#include <hal.h>

#include <vex/lcd.hpp>
#include <vex/digital.hpp>
#include <vex/vexspi.h>

int main(void) {
	// init chibios
	halInit();
	chSysInit();

	// init robot
	digital::setMode(1, 1);
	lcd::init();
	spi::init();

	int led = 1; // for led toggle

	while (1) {
		// update
		lcd::flush();
		spi::update();

		lcd::printn(0, 0, spi::getJoystick(1).Ch1);

		digital::setLed(1, led);
		led ^= 1;

		chThdSleepMilliseconds(100);
	}
}
