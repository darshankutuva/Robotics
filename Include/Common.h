/*
 *  Common.h
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

#ifndef _COMMON_H_
#define _COMMON_H_

//
// Change F_CPU value based on target processor frequency.
// Atmel AtMega 1284P Frequency = 8MHz.
//

#define F_CPU	8000000

#include <avr/io.h>

#include <avr/interrupt.h>

#include <util/delay.h>

#include <string.h>

#include <stdlib.h>

#include <math.h>

#define TRUE 	1
#define FALSE	0

typedef void * PVOID;

typedef unsigned char bool;

typedef enum _ANGLE_UNIT
{
	RADS	= 0,
	
	DEGREES	= 1
	
} ANGLE_UNIT;

typedef enum _ENABLE_DISABLE
{
	DISABLE		= 0,
	
	ENABLE		= 1
	
} ENABLE_DISABLE;

typedef union _SHORT_DATA
{
	struct
	{
		char LSB;
		
		char MSB;
	};
	
	short AsShort;
	
} SHORT_DATA, *PSHORT_DATA;

long
RoundLong(double Value);

#endif
