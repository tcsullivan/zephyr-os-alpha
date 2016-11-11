#include "lcd.hpp"

#include <ch.h>
#include <hal.h>

#include <library.hpp>

// lcd config for chibios
static SerialConfig lcdConfig = {
	19200,
	0,
	USART_CR2_STOP1_BITS,
	0
};

// buffer used to update lcd
static char lcdLine[32];

// signature for lcd buffer sent to LCD
static const char lcdBufBase[5] = {
	0xAA, 0x55, 0x1E, 0x12, 0,
};

// the lcd task, updates the LCD.
static char lcdUpdateWA[512];
static void lcdUpdate(void *unused)
{
	(void)unused;

	static int line = 2; // 2/3 for backlight

	// update function
	auto lcdLineUpdate = [&](char *s) {
		static char lcdBuf[22];
		// buffer prep
		strncpy(lcdBuf, lcdBufBase, 5);
		for (int i = 5; i < 21; i++)
			lcdBuf[i] = ' ';
		strncpy(lcdBuf + 5, s, 16);
		lcdBuf[4] = line;

		// checksum creation
		int cs = 0x100;
		for (int i = 4; i < 21; i++)
			cs -= lcdBuf[i];
		lcdBuf[21] = cs;
	
		// write to lcd
		sdWrite(&SD3, reinterpret_cast<unsigned char *>(lcdBuf), 22);
		chThdSleepMilliseconds(15);

		// read from lcd (buttons... (TODO))
		if (sdGetWouldBlock(&SD3))
			return;

		for (int i = 0; i < 16; i++) {
			auto c = sdGetTimeout(&SD3, TIME_IMMEDIATE);
			if (c != Q_TIMEOUT)
				lcdBuf[i] = c;
			else
				break;
		}
	};

	// loop on line updates
	while (1) {
		lcdLineUpdate(lcdLine);
		line++;
		lcdLineUpdate(lcdLine + 16);
		line--;
	}
}

namespace lcd {
	void init(void) {
		sdStart(&SD3, &lcdConfig);
		chThdCreateStatic(lcdUpdateWA, 512, NORMALPRIO - 1, lcdUpdate, nullptr);
	}

	void prints(int line, int x, const char *s) {
		strncpy(lcdLine + (16 * line) + x, s, 16 - x);
	}

	void printn(int line, int x, int val) {
		strncpy(lcdLine + (16 * line) + x, itoa(val), 16 - x);
	}
	
	void flush(void) {
		for (int i = 0; i < 32; i++)
			lcdLine[i] = ' ';
	}
}

