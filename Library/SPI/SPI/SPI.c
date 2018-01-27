 /*
 * SPI.c
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
 
#include "SPI.h"

#include "SPIInternal.h"

#include "Trace.h"

bool gSPIInitialized		= FALSE;

bool gIsMaster				= FALSE;

void
InitSPI (PSPI_MODE SPIMode)
{	
	
	//
	// Disable SPI
	//

	SPCR = 0;

	//
	// Reset Data Direction Register of SPI
	//

	DDRB &= ~((1 << PORTB4) | (1 << PORTB5) | (1 << PORTB6) | (1 << PORTB7));

	//
	// Set Clock Rate
	//

	SPCR = SPIMode->ClockRate & SPCR_CLOCK_RATE_MASK;

	if (SPIMode->ClockRate & SPSR_CLOCK_RATE_MASK)
	{
		SPSR = (1 << SPI2X);
	}

	if (SPIMode->ClockPulse)
	{
		SPCR |= (1 << CPHA);
	}

	//
	// Set Clock Polarity
	//

	if (SPIMode->ClockPolarity)
	{
		SPCR |= (1 << CPOL);
	}

	//
	// Set Clock Pulse
	//

	if (SPIMode->DataOrder)
	{
		SPCR |= (1 << DORD);
	}

	//
	// Set Direction Registers Accordingly.
	//
		
	if (SPIMode->IsMaster)
	{
		//
		// Master Mode
		//

		DDRB |= ((1 << PORTB4) | (1 << PORTB5) | (1 << PORTB7));

		SPCR |= (1 << MSTR);
			
		SS_HIGH();

		gIsMaster = TRUE;

	}
	else
	{
		//
		// Slave Mode
		//

		DDRB |= (1 << PORTB6);

	}

	//
	// Clear SPDR
	//
	
	SPDR = 0;

	//
	// Enable SPI
	//

	SPCR |= (1 << SPE);
	
	gSPIInitialized = TRUE;

}

void
SPIBeginTransfer()
{
	SS_LOW();
}

void
SPIEndTransfer()
{

	SS_HIGH();
}

bool
SPIMasterTransfer(PVOID TransmitBuffer, PVOID ReceiveBuffer, unsigned long Length) 
{
	bool Status = TRUE;
	
	unsigned long IndexI;
	
	unsigned char *TxBytePtr = (unsigned char *) TransmitBuffer;
	
	unsigned char *RxBytePtr = (unsigned char *) ReceiveBuffer;
	

	if (!gSPIInitialized)
	{

		SendTrace(ERROR_LEVEL, "SPITransfer - SPI Not Initialized\r\n");

		Status = FALSE;

		goto Exit;
	}
	
	if (!gIsMaster)
	{

		SendTrace(ERROR_LEVEL, "SPITransfer - SPI Not In Master Mode\r\n");

		Status = FALSE;

		goto Exit;
	}
	
	if (Length == 0 || TransmitBuffer == NULL || ReceiveBuffer == NULL) 
	{
		SendTrace(ERROR_LEVEL, "SPITransfer - SPI Buffer Error\r\n");
		
		Status = FALSE;
		
		goto Exit;
	}
	
	for (IndexI = 0; IndexI < Length; IndexI++)
	{
		SPDR = *TxBytePtr;
		
		//
		// Wait for transmission to complete
		//
		
		while (!(SPSR & (1 << SPIF)));
		
		//
		// Receive the data
		//
		
		*RxBytePtr = SPDR;
		
		//
		// Move to the next byte
		//
		
		TxBytePtr++;
		
		RxBytePtr++;
	}
	
Exit:

	return Status;
	
}
