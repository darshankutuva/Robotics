/*
 *  SPI.h
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
 
#ifndef _SPI_H_
#define _SPI_H_

#include "Common.h"

typedef enum _CLK_POL
{

	CLK_POL_0	= 0,

	CLK_POL_1	= 1

} CLK_POL;

typedef enum _CLK_PUL
{

	CLK_PUL_0	= 0,

	CLK_PUL_1	= 1

} CLK_PUL;

typedef enum _DATA_ORDER
{

	DATA_ORDER_MSB	= 0,

	DATA_ORDER_LSB	= 1

} DATA_ORDER;

typedef enum _CLK_RATE
{

	FRE_OSC_BY_4		= 0,
	
	FRE_OSC_BY_16		= 1,

	FRE_OSC_BY_64		= 2,

	FRE_OSC_BY_128		= 3,

	FRE_OSC_BY_2_2X		= 4,

	FRE_OSC_BY_8_2X		= 5,

	FRE_OSC_BY_32_2X	= 6,

	FRE_OSC_BY_64_2X	= 7

} CLK_RATE;

typedef struct _SPI_MODE
{

	bool		IsMaster;

	CLK_POL		ClockPolarity;

	CLK_PUL		ClockPulse;

	DATA_ORDER	DataOrder;

	CLK_RATE	ClockRate;

} SPI_MODE, *PSPI_MODE;

void
InitSPI (PSPI_MODE SPIMode);

void
SPIBeginTransfer();

void
SPIEndTransfer();

bool
SPIMasterTransfer(PVOID TransmitBuffer, PVOID ReceiveBuffer, unsigned long Length);

#endif
