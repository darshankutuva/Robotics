/*
 * Quadcopter.h
 *
 * Author: Shanthanand R Kutuva
 *
 * Copyright (C) 2015  Shanthanand R Kutuva darshankutuva@live.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 


#ifndef QUADCOPTER_H_
#define QUADCOPTER_H_

#include "Common.h"

#include "ExtInt.h"

#include "TWI.h"

#include "ADC.h"

#include "PWM.h"

#include "HMC5883L.h"

#include "Accelerometer.h"

#include "QuadcopterLib.h"

#include "Trace.h"

#include "Transceiver.h"

//
// Constant Definitions
//

#define NUM_INPUT_VALUES 4



//
// Define Transceiver Constants
//

#define MY_ADDRESS		0x7878787878

#define DEST_ADDRESS	0xa0b0c0d0e0

#define RF_CH			110

//
// MACROS
//

#define WaitForEvent(Event) {		\
									\
	while (!Event)					\
	{								\
		_delay_ms(5);				\
	}								\
									\
	Event = FALSE;					\
}

bool
InitTransmitter();

bool
PerformHandshake();

bool
InitializeGPS();

void
InitializeWatchDogTimer();

bool
SendCompassHeading();

void
FlyCopter();

#endif /* QUADCOPTER_H_ */