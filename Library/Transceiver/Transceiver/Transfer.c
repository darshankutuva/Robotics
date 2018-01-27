/*
 *  Transfer.c
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

#include "Common.h"

#include "TransceiverInternal.h"

#include "Trace.h"

void
TCInitTransfer ()
{
#ifdef ATMEGA_1284P
	
	//
	// SPI Initialization Code for ATMega 1284P
	
	SPI_MODE SPIMode;
	
	SPIMode.IsMaster		= TRUE;

	SPIMode.ClockPolarity	= CLK_POL_0;
	
	SPIMode.ClockPulse		= CLK_PUL_0;
	
	SPIMode.DataOrder		= DATA_ORDER_MSB;
	
	SPIMode.ClockRate		= FRE_OSC_BY_4;
	
	InitSPI(&SPIMode);
	
#else

	//
	// SPI Initialization Code for Galileo
	//

#endif
}