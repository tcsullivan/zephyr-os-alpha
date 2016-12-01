#include <ch.h>
#include <hal.h>

#include <vex/lcd.hpp>
#include <vex/digital.hpp>
#include <vex/vexspi.h>

static bool shouldRun = true;

static char modeMonitorWA[512];
static char autonWA[512];
static char opconWA[512];

void auton(void *);
void opcon(void *);

void modeMonitor(void *unused)
{
	(void)unused;

	static int ut = 1;
	while (1) {
		chThdSleepMilliseconds(16);
		digital::setLed(2, (ut ^= 1));

		auto state = spi::getState();
		// if not disabled
		if (!(state & 0x80)) {
			shouldRun = true;

			// if autonomous
			if (state & 0x40) {
				digital::setLed(3, 1);
				chThdCreateStatic(autonWA, 512, NORMALPRIO - 1, auton, nullptr);
				state = 0x40;
			} else {
				chThdCreateStatic(opconWA, 512, NORMALPRIO - 1, opcon, nullptr);
				state = 0;
			}

			while ((spi::getState() & (0xC0)) == state)
				chThdSleepMilliseconds(16);

			// TODO stop all
			shouldRun = false;
		} else digital::setLed(3, 0);
	}
}

void auton(void *unused)
{
	(void)unused;

	static int led = 1;

	while (shouldRun) {
		digital::setLed(1, led);
		led ^= 1;

		chThdSleepMilliseconds(100);
	}
}

void opcon(void *unused)
{
	(void)unused;

	while (shouldRun) {
		lcd::flush();
		lcd::printn(0, 0, spi::getJoystick(1).Ch1);

		chThdSleepMilliseconds(100);
	}
}

int main(void)
{
	// init chibios
	halInit();
	chSysInit();

	// init robot
	lcd::init();
	spi::init();
	
	digital::setMode(1, 1);
	digital::setMode(2, 1);
	digital::setMode(3, 1);

	chThdCreateStatic(modeMonitorWA, 512, NORMALPRIO - 1, modeMonitor, nullptr);

	while (1) {
		// update
		spi::update();
		chThdSleepMilliseconds(10);
	}
}
