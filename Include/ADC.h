/*
 * ADC.h
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

#ifndef ADC_H
#define ADC_H

#include "Common.h"

#define ADC_MAX_VALUE		1020

#define ADC_CENTER			(ADC_MAX_VALUE / 2.0)

typedef enum _ADC_CHANNEL
{
	ADC_CHANNEL_0		= 0,
	
	ADC_CHANNEL_1		= 1,
	
	ADC_CHANNEL_2		= 2,
	
	ADC_CHANNEL_3		= 3,
	
	ADC_CHANNEL_4		= 4,
	
	ADC_CHANNEL_5		= 5,
	
	ADC_CHANNEL_6		= 6,
	
	ADC_CHANNEL_7		= 7,
	
	ADC_MAX_CHANNELS	= 8

} ADC_CHANNEL;


typedef enum _ADC_REF_VOLT
{
	AREF_VOLT			= 0x0,
	
	AVCC_VOLT			= 0x1,
	
	INT_1_1_VOLT		= 0x2,
	
	INT_2_56_VOLT		= 0x3
	
} ADC_REF_VOLT;

typedef enum _ADC_PRE_SCALAR
{
	DIV_FACTOR_0		= 0,
	
	DIV_FACTOR_2		= 1,
	
	DIV_FACTOR_4		= 2,
	
	DIV_FACTOR_8		= 3,
	
	DIV_FACTOR_16		= 4,
	
	DIV_FACTOR_32		= 5,
	
	DIV_FACTOR_64		= 6,
	
	DIV_FACTOR_128		= 7
	
} ADC_PRE_SCALAR;

typedef union _ADC_FLAGS
{
	struct
	{
		unsigned char IsLeftAdjustResult	: 1;
		
		unsigned char Rsvd					: 7;
	};
	
	unsigned char AsUINT8;
	
} ADC_FLAGS, *PADC_FLAGS;

typedef struct _ADC_INIT_PARAMS
{
	ADC_FLAGS			Flags;

	ADC_REF_VOLT 		RefVoltSelect;
	
	ADC_PRE_SCALAR		Prescalar;
	
	unsigned char 		EnabledChannelsBitVector;
	
} ADC_INIT_PARAMS, *PADC_INIT_PARAMS;

//
// Public Functions
//

bool
InitADC(PADC_INIT_PARAMS InitParams);

bool
ADCRead(ADC_CHANNEL AdcChannel, unsigned int *Value);

#endif
