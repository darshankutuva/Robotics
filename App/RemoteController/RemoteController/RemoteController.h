/*
 * RemoteController.h
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


#ifndef REMOTECONTROLLER_H_
#define REMOTECONTROLLER_H_

#include "Common.h"

#include "Trace.h"

#include "Transceiver.h"

#include "ADC.h"

#include "TWI.h"

#include "LCD.h"

//
// Constant Definitions
//

#define NUM_INPUT_VALUES 4

//
// Define LCD Constants
//

#define LCD_ADDRESS		0x27

#define MAX_CHARACTERS  0x10

#define MAX_LINES		0x2

//
// Define Transceiver Constants
//

#define MY_ADDRESS		0xa0b0c0d0e0

#define DEST_ADDRESS	0x7878787878

#define RF_CH			110

//
// Macro Definitions
//

#define WaitForEvent(Event) {		\
									\
	while (!Event)					\
	{								\
		_delay_ms(10);				\
	}								\
									\
	Event = FALSE;					\
}

//
// Function declarations
//

bool
InitDisplay();

bool
InitTransmitter();

bool
InitInputControl();

void
WaitForThrottle();

bool
ConnectToCopter();

void
FlyCopter();

bool
VerifyAckData();

void
UpdateLCD();

bool
GetDirection(double CompassHeading, char *Direction, unsigned char Length);

void
RCADCIntCallback(PVOID CallbackContext);

#endif /* REMOTECONTROLLER_H_ */