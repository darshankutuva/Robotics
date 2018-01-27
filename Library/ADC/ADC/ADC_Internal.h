/*
 * ADC_Internal.h
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

#ifndef ADC_INTERNAL_H
#define ADC_INTERNAL_H

#include "ADC.h"

#define START_CONVERSION_BIT	0x6

#define START_CONVERSION()  (ADCSRA |= (0x1 << START_CONVERSION_BIT))

#define INT_FLAG_BIT			0x4

#define WAIT_FOR_CONVERSION() {						\
	while(!(ADCSRA & (0x1 << INT_FLAG_BIT)));		\
}

typedef union _ADC_MUX
{
	struct
	{
		unsigned char Mux 				: 5;
		
		unsigned char Adlar				: 1;
		
		unsigned char RefVoltSelect		: 2;
	};
	
	unsigned char AsUINT8;
	
} ADC_MUX, *PADC_MUX;

typedef union _ADC_CTRL_STATUS_A
{
	struct
	{
		unsigned char Prescalar 		: 3;
		
		unsigned char IntEnable			: 1;
		
		unsigned char IntFlag			: 1;
		
		unsigned char AutoTrigEnable	: 1;
		
		unsigned char StartConversion	: 1;
		
		unsigned char ADCEnable			: 1;
	};
	
	unsigned char AsUINT8;
	
} ADC_CTRL_STATUS_A, *PADC_CTRL_STATUS_A;

#endif