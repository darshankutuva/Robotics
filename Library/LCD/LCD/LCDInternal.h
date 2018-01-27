/*
 * LCDInternal.h
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


#ifndef LCDINTERNAL_H_
#define LCDINTERNAL_H_

#include "LCD.h"

#define UPPER_NIBBLE_SHIFT			0x4

#define LOWER_NIBBLE_MASK			0xf

#define MAX_BUFFER_SIZE				0x40

//
// Clear Display
//

#define CLEAR_DISPLAY_CTRL				0x1

//
// Return Home
//

#define RETURN_HOME_CTRL				0x2

//
// Entry Mode Set
//

#define ENTRY_MODE_CTRL					0x4

#define EM_DISPLAY_SHIFT				0x1

#define INCREMENT_POS					0x2

//
// Display ON/OFF Control
//

#define DISPLAY_CTRL					0x8

#define BLINK_ON						0x1

#define CURSOR_ON						0x2

#define DISPLAY_ON						0x4

//
// Cursor or Display Shift
//

#define CURSOR_DISPLAY_SHIFT_CTRL		0x10

#define CD_DISPLAY_SHIFT				0x8

#define MOVE_RIGHT						0x4

//
// Function Set
//

#define FUNC_SET_CTRL					0x20

#define DATA_LEN_8BITS					0x10

#define TWO_LINES						0x8

#define FONT_5X10						0x4

//
// CGRAM
//

#define CGRAM_CTRL						0x40

//
// DDRAM
//

#define DDRAM_CTRL						0x80

#define DDRAM_ADDRESS_INTERLEAVING		0x40

typedef enum _LCD_REG_SELECT
{
	LCD_IR_REG		= 0,
	
	LCD_DR_REG		= 1
		
} LCD_REG_SELECT;

typedef union _LCD_TWI_DATA
{
	struct
	{			
		unsigned char RegSelect : 1;
		
		unsigned char ReadWrite : 1;
		
		unsigned char Enable	: 1;
		
		unsigned char BackLight : 1;
		
		unsigned char Data		: 4;
	};
	
	unsigned char AsUINT8;
	
} LCD_TWI_DATA, *PLCD_TWI_DATA;

//
// Address Counter
//

typedef union _ADDR_CTR_BUSY_FLAG
{
	struct
	{
		unsigned char AddrCtr : 7;
		
		unsigned char BusyFlag : 1;
	};
	
	unsigned char AsUINT8;
	
} ADDR_CTR_BUSY_FLAG, *PADDR_CTR_BUSY_FLAG;

//
// Private Functions
//

void
LCDStartUp();

bool
LCDSetEntryMode(bool IsDisplayShift, bool IsIncrement);

bool
LCDShiftCursorOrDisplay(bool IsDisplayShift, bool IsIncrement);

bool
LCDSetFunctionControl(bool IsTwoLineDisplay, bool Is5X10Font);

bool
LCDTransmit(unsigned char Value, LCD_REG_SELECT RegSelect, bool IsEndOfTransfer);

bool
LCDWrite(PLCD_TWI_DATA TwiData, bool IsEndOfTransfer);

#endif /* LCDINTERNAL_H_ */