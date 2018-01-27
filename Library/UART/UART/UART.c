/*
 * UART.c
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

#include "UART.h"

#define MAX_WAIT_TIME			32

#define TIMER_INTERVAL			7.8

//
// Global Variable Definition
//

PRCI_CALLBACK gRCICallback0;

PRCI_CALLBACK gRCICallback1;

PTCI_CALLBACK gTCICallback0;

PTCI_CALLBACK gTCICallback1;

PVOID gRCICallbackContext0;

PVOID gRCICallbackContext1;

PVOID gTCICallbackContext0;

PVOID gTCICallbackContext1;

bool gUARTInitialized[MAX_UART] = {FALSE, FALSE};
	
bool gUARTTimerTriggered;

bool gIsMPCMEnabled[MAX_UART] = {FALSE, FALSE};

//
// Private Function Declaration
//

void
InitializeUARTTimer();

//
// Macro Definition
//

#define StartUARTTimer(WaitTimeInMs)			\
{												\
	gUARTTimerTriggered = FALSE;				\
												\
	TCCR0B = (0x1 << CS02) | (0x1 << CS00);		\
												\
	OCR0A = abs(WaitTimeInMs * TIMER_INTERVAL);	\
												\
	TCNT0 = 0;									\
												\
}

#define StopUARTTimer()							\
{												\
	TCCR0B = 0x0;								\
}

bool
InitUARTSync(UART_NUMBER UARTNumber, BAUD_RATE BaudRate, UART_MODE UARTMode)
{

	bool Status = TRUE;
	
	gIsMPCMEnabled[UARTNumber] = UARTMode;
	
	switch (UARTNumber)
	{

		case 0:
		{
			UBRR0H = (unsigned char) (BaudRate >> 8);
	
			UBRR0L = (unsigned char) BaudRate;
	
			//
			// USART Control and Status Register n B.
			//
	
			//
			// Enable Receiver and Transmitter.
			//
	
			UCSR0B = ((0x1 << TXEN0) | (0x1 << RXEN0) | (0x1 << RXCIE0));
	
			//
			// USART Control and Status Register A
			// Set MPCM mode
			//

			if (UARTMode == MULTI_PROCESSOR_COMMUNICATION_MODE)
			{
				//
				// Asynchronous USART, Parity Disabled, Stop Bits - 1, Character Size - 9 bits.
				//
				
				UCSR0C = ((0x1 << UCSZ00) | (0x1 << UCSZ01) | (0x1 << UCSZ02));
				
				UCSR0A = (0x1 << MPCM0);
			}
			else
			{
				//
				// Normal Mode
				// Asynchronous USART, Parity Disabled, Stop Bits - 1, Character Size - 8 bits.
				//
				
				UCSR0C = ((0x1 << UCSZ00) | (0x1 << UCSZ01));
			}

			gUARTInitialized[0] = TRUE;

			break;
	
		}
		case 1:
		{

			UBRR1H = (unsigned char) (BaudRate >> 8);
	
			UBRR1L = (unsigned char) BaudRate;
	
			//
			// USART Control and Status Register n B.
			//
	
			//
			// Enable Receiver and Transmitter.
			//
	
			UCSR1B = ((0x1 << TXEN1) | (0x1 << RXEN1) | (0x1 << RXCIE1));
	
			//
			// USART Control and Status Register A.
			// Set MPCM Mode
			//

			if (UARTMode == MULTI_PROCESSOR_COMMUNICATION_MODE)
			{
				// Asynchronous USART, Parity Disabled, Stop Bits - 1, Character Size - 9 bits.
				//
				
				UCSR1C = ((0x1 << UCSZ10) | (0x1 << UCSZ11) | (0x1 << UCSZ12));
				
				UCSR1A = (0x1 << MPCM1);
			}
			else
			{
				// 
				// Normal Mode
				// Asynchronous USART, Parity Disabled, Stop Bits - 1, Character Size - 8 bits.
				//
				
				UCSR1C = ((0x1 << UCSZ10) | (0x1 << UCSZ11));
			}

			gUARTInitialized[1] = TRUE;

			break;

		}
		default:
		{
			Status = FALSE;

			break;
		}
	}
	
	InitializeUARTTimer();

	return Status;

}

unsigned long
UARTTransmitAddress(UART_NUMBER UARTNumber, unsigned char Address)
{
	//
	// Check if bit 9 is enabled
	//
	
	if (!gIsMPCMEnabled[UARTNumber])
	{
		return 0;
	}
	
	return UARTTransmit(UARTNumber, &Address, sizeof(unsigned char), TRUE);
}

unsigned long
UARTTransmitSync(UART_NUMBER UARTNumber, PVOID TransmitValue, unsigned long Length)
{
	return UARTTransmit(UARTNumber, TransmitValue, Length, FALSE);
}

unsigned long
UARTReceiveASync(UART_NUMBER UARTNumber, PVOID ReceiveValue, unsigned long Length, unsigned char WaitTimeInMs)
{
	return UARTReceive(UARTNumber, ReceiveValue, Length, WaitTimeInMs);
}

unsigned long
UARTReceiveSync(UART_NUMBER UARTNumber, PVOID ReceiveValue, unsigned long Length)
{
	return UARTReceive(UARTNumber, ReceiveValue, Length, 0);
}

bool
UARTEnableDisableReceiver(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable)
{
	bool Status = FALSE;
	
	switch (UARTNumber)
	{
		case 0:
		{

			if (!gUARTInitialized[0])
			{
				goto Exit;
			}
	
			if (EnableDisable)
			{
				UCSR0B |= (0x1 << RXEN0);
			}
			else
			{
				UCSR0B &= ~(0x1 << RXEN0);
			}
		}
		case 1:
		{

			if (!gUARTInitialized[1])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				UCSR1B |= (0x1 << RXEN1);
			}
			else
			{
				UCSR1B &= ~(0x1 << RXEN1);
			}
		}
		default:
		{
			break;
		}
	}
	
	Status = TRUE;
	
Exit:

	return Status;
}

bool
UARTEnableDisableTransmitter(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable)
{
	bool Status = FALSE;
	
	switch (UARTNumber)
	{
		case 0:
		{

			if (!gUARTInitialized[0])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				UCSR0B |= (0x1 << TXEN0);
			}
			else
			{
				UCSR0B &= ~(0x1 << TXEN0);
			}
		}
		case 1:
		{

			if (!gUARTInitialized[1])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				UCSR1B |= (0x1 << TXEN1);
			}
			else
			{
				UCSR1B &= ~(0x1 << TXEN1);
			}
		}
		default:
		{
			break;
		}
	}
	
	Status = TRUE;
	
Exit:

	return Status;
}

bool 
UARTEnableDisableMPCM(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable)
{
	bool Status = FALSE;
	
	//
	// Check if bit 9 is enabled
	//
	
	if (!gIsMPCMEnabled[UARTNumber])
	{
		goto Exit;
	}
	
	switch (UARTNumber)
	{
		case 0:
		{
			if (!gUARTInitialized[0])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				UCSR0A |= (0x1 << MPCM0);
			}
			else
			{
				UCSR0A &= ~(0x1 << MPCM0);
			}
		}
		case 1:
		{

			if (!gUARTInitialized[1])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				UCSR1A |= (0x1 << MPCM1);
			}
			else
			{
				UCSR1A &= ~(0x1 << MPCM1);
			}
		}
		default:
		{
			break;
		}
	}
	
	Status = TRUE;

Exit:

	return Status;
}

bool
UARTEnableDisableReceiveCompleteInterrupt(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable, PRCI_CALLBACK pRCICallback, PVOID pCallbackContext)
{
	bool Status = FALSE;
	
	switch (UARTNumber)
	{
		case 0:
		{
			if (!gUARTInitialized[0])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				if (pRCICallback == NULL)
				{
					goto Exit;
				}
				
				UCSR0B |= (0x1 << RXCIE0);
				
				gRCICallback0 = pRCICallback;
				
				gRCICallbackContext0 = pCallbackContext;
			}
			else
			{
				UCSR0B &= ~(0x1 << RXCIE0);
				
				gRCICallback0 = NULL;
				
				gRCICallbackContext0 = NULL;
			}
		}
		case 1:
		{

			if (!gUARTInitialized[1])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				if (pRCICallback == NULL)
				{
					goto Exit;
				}
				
				UCSR1B |= (0x1 << RXCIE1);
				
				gRCICallback1 = pRCICallback;
				
				gRCICallbackContext1 = pCallbackContext;
			}
			else
			{
				UCSR1B &= ~(0x1 << RXCIE1);
				
				gRCICallback1 = NULL;
				
				gRCICallbackContext1 = NULL;
			}
		}
		default:
		{
			break;
		}
	}
	
	Status = TRUE;

Exit:

	return Status;
}

bool
UARTEnableDisableTransmitCompleteInterrupt(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable, PTCI_CALLBACK pTCICallback, PVOID pCallbackContext)
{
	bool Status = FALSE;
	
	switch (UARTNumber)
	{
		case 0:
		{
			if (!gUARTInitialized[0])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				if (pTCICallback == NULL)
				{
					goto Exit;
				}
				
				UCSR0B |= (0x1 << TXCIE0);
				
				gTCICallback0 = pTCICallback;
				
				gTCICallbackContext0 = pCallbackContext;
			}
			else
			{
				UCSR0B &= ~(0x1 << TXCIE0);
				
				gTCICallback0 = NULL;
				
				gTCICallbackContext0 = NULL;
			}
		}
		case 1:
		{

			if (!gUARTInitialized[1])
			{
				goto Exit;
			}
			
			if (EnableDisable)
			{
				if (pTCICallback == NULL)
				{
					goto Exit;
				}
				
				UCSR1B |= (0x1 << TXCIE1);
				
				gTCICallback1 = pTCICallback;
				
				gTCICallbackContext1 = pCallbackContext;
			}
			else
			{
				UCSR1B &= ~(0x1 << TXCIE1);
				
				gTCICallback1 = NULL;
				
				gTCICallbackContext1 = NULL;
			}
		}
		default:
		{
			break;
		}
	}
	
	Status = TRUE;

Exit:

	return Status;
}

//
// Private Functions
//

void
InitializeUARTTimer()
{
	//
	// Initialize Timer Counter 0 as watch dog timer.
	//
	
	//
	// Set Timer/Counter Control Register A to normal mode of operation.
	//
	
	TCCR0A = (0x1 << WGM01);
	
	//
	// Set Timer/Counter Control Register B to have a clock with pre-scalar = 1024.
	// The timer will expire every 32.64 ms.
	//
	
	TCCR0B = 0;
	
	//
	// Set Timer/Counter Register to 0xFF
	//
	
	TCNT0 = 0x0;
	
	//
	// Set OCR0A and OCR0B to zero
	//
	
	OCR0A = 0;
	
	OCR0B = 0;
	
	//
	// Enable Timer/Counter0 compare Interrupt
	//
	
	TIMSK0 = (0x1 << OCIE0A);
}

unsigned long
UARTTransmit(UART_NUMBER UARTNumber, PVOID TransmitValue, unsigned long Length, bool IsAddress)
{
	unsigned char *Value = (unsigned char *) TransmitValue;
	
	unsigned long IndexI = 0;
	
	switch (UARTNumber)
	{
		case 0:
		{
			if (!gUARTInitialized[0])
			{
				goto Exit;
			}

			for (IndexI = 0; IndexI < Length; IndexI++)
			{
				while (!(UCSR0A & (1 << UDRE0)));

				//
				// Check if bit 9 is enabled.
				//

				if (gIsMPCMEnabled[0]) 
				{
					if (IsAddress)
					{
						UCSR0B |= (0x1 << TXB80);
					}
					else
					{
						UCSR0B &= ~(0x1 << TXB80);
					}
				}

				//
				// Transmit the value
				//

				UDR0 = *Value;

				//
				// Move to next value
				//

				Value++;

			}

			break;

		}
		case 1:
		{
			if (!gUARTInitialized[1])
			{
				goto Exit;
			}

			for (IndexI = 0; IndexI < Length; IndexI++)
			{

				while (!(UCSR1A & (1 << UDRE1)));

				//
				// Check if bit 9 is enabled.
				//

				if (gIsMPCMEnabled[1])
				{
					if (IsAddress)
					{
						UCSR1B |= (0x1 << TXB81);
					}
					else
					{
						UCSR1B &= ~(0x1 << TXB81);
					}		
				}

				//
				// Transmit the value
				//

				UDR1 = *Value;

				//
				// Move to next value
				//

				Value++;

			}

			break;
		}
		default:
		{
			break;
		}
	}

Exit:

	return IndexI;

}

unsigned long
UARTReceive(UART_NUMBER UARTNumber, PVOID ReceiveValue, unsigned long Length, unsigned char WaitTimeInMs)
{
	
	unsigned char *Value = (unsigned char *) ReceiveValue;

	unsigned char BitNine;

	unsigned long IndexI = 0;
	
	if (WaitTimeInMs > MAX_WAIT_TIME)
	{
		goto Exit;
	}

	switch (UARTNumber)
	{
		case 0:
		{

			if (!gUARTInitialized[0])
			{
				goto Exit;
			}

			for (IndexI = 0; IndexI < Length; IndexI++)
			{

				if(WaitTimeInMs > 0)
				{
					
					StartUARTTimer(WaitTimeInMs);

					while (!(UCSR0A & (1 << RXC0)))
					{
						if (gUARTTimerTriggered)
						{
							StopUARTTimer();
						
							goto Exit;
						}
					}
				
					StopUARTTimer();
				}
				else
				{
					while (!(UCSR0A & (1 << RXC0)));
				}

				//
				// Receive the value
				//

				if (gIsMPCMEnabled[0])
				{
					BitNine = UCSR0B & (0x1 << RXB80);
						
					BitNine = BitNine >> RXB80;
				}

				*Value = UDR0;

				//
				// Move to the next byte.
				//

				Value++;

			}

			break;
		}
		case 1:
		{

			if (!gUARTInitialized[1])
			{
				goto Exit;
			}

			for (IndexI = 0; IndexI < Length; IndexI++)
			{
				
				if (WaitTimeInMs > 0)
				{
					
					StartUARTTimer(WaitTimeInMs);

					while (!(UCSR1A & (1 << RXC1)))
					{
						if (gUARTTimerTriggered)
						{
							StopUARTTimer();
						
							goto Exit;
						}
					}

					StopUARTTimer();
				}
				else
				{
					while (!(UCSR1A & (1 << RXC1)));
				}

				//
				// Receive the value
				//

				if (gIsMPCMEnabled[1])
				{
					BitNine = UCSR1B & (0x1 << RXB81);
						
					BitNine = BitNine >> RXB81;
				}

				*Value = UDR1;

				//
				// Move to next byte.
				//

				Value++;

			}

			break;
		}
		default:
		{
			break;
		}
	}

Exit:

	return IndexI;

}

ISR(TIMER0_COMPA_vect)
{
	gUARTTimerTriggered = TRUE;
}

ISR(USART0_RX_vect)
{
	if(gRCICallback0 != NULL)
	{
		gRCICallback0(UART_ZERO, gRCICallbackContext0);
	}
}

ISR(USART1_RX_vect)
{
	if(gRCICallback1 != NULL)
	{
		gRCICallback1(UART_ONE, gRCICallbackContext1);
	}
}

ISR(USART0_TX_vect)
{
	if(gTCICallback0 != NULL)
	{
		gTCICallback0(UART_ZERO, gTCICallbackContext0);
	}
}

ISR(USART1_TX_vect)
{
	if(gTCICallback1 != NULL)
	{
		gTCICallback1(UART_ONE, gTCICallbackContext1);
	}
}