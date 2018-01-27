/*
 *  ExtInt.h
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

#ifndef _EXT_INT_H_
#define _EXT_INT_H_

#include "Common.h"

typedef void (*PEXTINT_CALLBACK) (PVOID CallbackContext);

typedef enum _EXTINT_NUM
{
	EXTINT_INT0		= 0,
	
	EXTINT_INT1		= 1,
	
	EXTINT_INT2		= 2,
	
	MAX_EXTINTS		= 3
	
} EXTINT_NUM;

typedef enum _EXTINT_POLARITY
{
	LEVEL_TRIGGERED	 		= 0,
	
	EDGE_TRIGGERED	 		= 1,
	
	FALLING_EDGE_TRIGGERED	= 2,
	
	RISING_EDGE_TRIGGERED	= 3
	
} EXTINT_POLARITY;

//
// Pulblic Functions
//

void
InitExtInt();

bool
RegisterExtInterrupt(EXTINT_NUM ExtIntNum, EXTINT_POLARITY Polarity, PEXTINT_CALLBACK Callback, PVOID CallbackContext, bool EnableInternalPullup);

bool
UnRegisterExtInterrupt(EXTINT_NUM ExtIntNum);

bool
MaskExtInterrupt(EXTINT_NUM ExtIntNum);

bool
UnMaskExtInterrupt(EXTINT_NUM ExtIntNum);



#endif