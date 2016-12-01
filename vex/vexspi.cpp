/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2013                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     vexspi.c                                                     */
/*    Author:     James Pearman                                                */
/*    Created:    7 May 2013                                                   */
/*                                                                             */
/*    Revisions:                                                               */
/*                V1.00     4 July 2013 - Initial release                      */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    This file is part of ConVEX.                                             */
/*                                                                             */
/*    The author is supplying this software for use with the VEX cortex        */
/*    control system. ConVEX is free software; you can redistribute it         */
/*    and/or modify it under the terms of the GNU General Public License       */
/*    as published by the Free Software Foundation; either version 3 of        */
/*    the License, or (at your option) any later version.                      */
/*                                                                             */
/*    ConVEX is distributed in the hope that it will be useful,                */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*    GNU General Public License for more details.                             */
/*                                                                             */
/*    You should have received a copy of the GNU General Public License        */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*                                                                             */
/*    A special exception to the GPL can be applied should you wish to         */
/*    distribute a combined work that includes ConVEX, without being obliged   */
/*    to provide the source code for any proprietary components.               */
/*    See the file exception.txt for full details of how and when the          */
/*    exception can be applied.                                                */
/*                                                                             */
/*    The author can be contacted on the vex forums as jpearman                */
/*    or electronic mail using jbpearman_at_mac_dot_com                        */
/*    Mentor for team 8888 RoboLancers, Pasadena CA.                           */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

#include "ch.h"         // needs for all ChibiOS programs
#include "hal.h"        // hardware abstraction layer header

#include <vex/vexspi.h>
#include <library.hpp>

SpiData vexSpiData; // externed in main.cpp

constexpr GPTDriver *spiGpt = &GPTD2;
constexpr const char *spiTeamName = CONVEX_TEAM_NAME;

static thread_t *spiThread = nullptr;

static SPIConfig spicfg = {
    nullptr,
    /* HW dependent part.*/
    VEX_SPI_CS_PORT, VEX_SPI_CS_PIN,
    SPI_CR1_DFF | SPI_CR1_BR_2 | SPI_CR1_CPHA
};

static void _vspi_gpt_cb(GPTDriver *);
static const GPTConfig vexSpiGpt = {
	1000000,      /* 1MHz timer clock.*/
	_vspi_gpt_cb, /* Timer callback.*/
	0,            /* DIER = 0, version 2.6.1.and on */
	0
};

static const unsigned char txInitData[32] = {
	0x17, 0xC9, 0x02, 0x00, 0x00, 0x00,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00
};

static void _vspi_gpt_cb(GPTDriver *gptp)
{
	(void)gptp;

	chSysLockFromISR();
	if (spiThread != nullptr) {
		chSchReadyI(spiThread);
		spiThread = nullptr;
	}
	chSysUnlockFromISR();
}

void vexSpiTickDelay(int ticks)
{
	chSysLock();
	spiThread = chThdGetSelfX();
	gptStartOneShotI(spiGpt, ticks);
	chSchGoSleepS(CH_STATE_SUSPENDED);
	chSysUnlock();
}



namespace spi {
	void init(void) {
		memncpy(&vexSpiData.txdata, txInitData, 32);
		vexSpiData.online = 0;

		spiStart(&SPID1, &spicfg);
		gptStart(spiGpt, &vexSpiGpt);
	}


	void update(void) {
		auto txbuf = reinterpret_cast<uint16_t *>(vexSpiData.txdata.data);
		auto rxbuf = reinterpret_cast<uint16_t *>(vexSpiData.rxdata_t.data);

		// configure team name
		if (vexSpiData.txdata.pak.state == 0x03) {
			// let spi know what we're sending
			vexSpiData.txdata.pak.type = 0x55;
			strncpy(reinterpret_cast<char *>(vexSpiData.txdata.data + 6), spiTeamName, 8);
		}

	    // Set handshake to indicate new spi message
	    palSetPad(VEX_SPI_ENABLE_PORT, VEX_SPI_ENABLE_PIN);

	    for (int i = 0; i < 16; i++) {
			// exchange word (int)
	        spiSelectI(&SPID1);
	        rxbuf[i] = spi_lld_polled_exchange( &SPID1, txbuf[i] );
	        spiUnselectI(&SPID1);

			if (((i%4) == 3) && (i != 15)) {
	            // long delay between each group of 4 words
	            vexSpiTickDelay(73);

	            // After 4 words negate handshake pin
	            palClearPad(VEX_SPI_ENABLE_PORT, VEX_SPI_ENABLE_PIN);
	        } else {
	        	vexSpiTickDelay(8);
			}
	    }

	    // increase id for next message
	    vexSpiData.txdata.pak.id++;

	    // check integrity of received data
	    if( (vexSpiData.rxdata_t.data[0] == 0x17 ) && (vexSpiData.rxdata_t.data[1] == 0xC9 ))
	        {
	        // copy temporary data
	        for(int i=0;i<32;i++)
	            vexSpiData.rxdata.data[i] = vexSpiData.rxdata_t.data[i];

	        // Set online status if valid data status set
	        if( (vexSpiData.rxdata.pak.status & 0x0F) == 0x08 )
    	        vexSpiData.online = 1;

        	// If in configuration initialize state (0x02 or 0x03)
	        if( (vexSpiData.txdata.pak.state & 0x0E) == 0x02 )
	            {
	            // check for configure request
	            if( (vexSpiData.rxdata.pak.status & 0x0F) == 0x02 )
	                vexSpiData.txdata.pak.state = 0x03;
	            // check for configure and acknowledge
	            if( (vexSpiData.rxdata.pak.status & 0x0F) == 0x03 )
	                {
	                vexSpiData.txdata.pak.state = 0x08;
	                vexSpiData.txdata.pak.type  = 0;
	                }
	            // Either good or bad data force to normal transmission
	            // status will either be 0x04 or 0x08
	            if( (vexSpiData.rxdata.pak.status & 0x0C) != 0x00 )
	                {
	                vexSpiData.txdata.pak.state = 0x08;
	                vexSpiData.txdata.pak.type  = 0;
	                }
    	        }
	        }
	    else
	        vexSpiData.errors++;
	}

	void setMotors(uint8_t *data) {
		memncpy(data, vexSpiData.txdata.pak.motor, 8);
	}

	const jsdata& getJoystick(int num) {
		return (num == 2) ? vexSpiData.rxdata.pak.js_2 : vexSpiData.rxdata.pak.js_1;
	}

	int getBatteryMain(void) {
		return vexSpiData.rxdata.pak.batt1 * 59; // in mV
	}

	int getBatteryBackup(void) {
		return vexSpiData.rxdata.pak.batt2 * 59; 
	}

	int getState(void) {
		return vexSpiData.rxdata.pak.ctl;
	}

}
