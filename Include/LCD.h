/*
 *  LCD.h
 *
 *  Author: Shanthanand R Kutuva
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
 
#ifndef _LCD_H_
#define _LCD_H_

#include "Common.h"

#include "TWI.h"

//
// LCD_INIT_PARAMS
//

typedef union LCD_INIT_FLAGS
{
	struct
	{
		unsigned int IsDisplayShift			: 1;
		
		unsigned int IsIncrementPos			: 1;
		
		unsigned int IsDisplayOn			: 1;
		
		unsigned int IsCursorOn				: 1;
		
		unsigned int IsCursorBlinkOn		: 1;
		
		unsigned int Is5X10Font				: 1;
		
		unsigned int IsBackLightOn			: 1;
		
		unsigned int IsAutoScroll			: 1;
		
		unsigned int Rsvd					: 7;
	};
	
	unsigned int AsUINT16;
	
} LCD_INIT_FLAGS, *PLCD_INIT_FLAGS;

typedef struct LCD_INIT_PARAMS
{
	LCD_INIT_FLAGS	Flags;
	
	unsigned char MaxLines;
	
	unsigned char MaxCharacters;
	
	TWI_ADDRESS SlaveAddress;
	
} LCD_INIT_PARAMS, *PLCD_INIT_PARAMS;

//
// Public Functions
//

bool
InitLCD(PLCD_INIT_PARAMS InitParams);

bool
LCDClearDisplay();

bool
LCDSetCursorToHomePos();

bool
LCDSetDisplayControl(bool IsDisplayOn, bool IsCursorOn, bool IsCursorBlinkOn, bool IsBackLightOn);

bool
LCDSetCursorPosition(unsigned char LineNumber, unsigned char Position);

bool
LCDPrint(char *String);

#endif
