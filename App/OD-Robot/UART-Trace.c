#define F_CPU 1000000UL

#include <avr/io.h>

#include "UART-Trace.h"

void InitUART()
{

	//
	// Set UBRRH to 0
	//

	UBRRH = BAUD_RATE_9600 >> 8;

	//
	// Set the baud rate first.
	//

	UBRRL = BAUD_RATE_9600;

	//
	// Enable Transmission
	//

	UCSRB = (1 << TXEN);
	
	//
	// Set Character Size in UCSRC.
	//

	UCSRC = ((1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0));	

}

void 
SendTrace(unsigned int Value)
{

	unsigned char TransmitChar[10];

	unsigned char Count = 0;

	do
	{

		if (Value < 10)
		{
			TransmitChar[Count] = Value;

			Value = 0;
		}	
		else
		{
			TransmitChar[Count] = Value % 10;

			Value = Value / 10;

		}
			
		Count++;  

	} while (Value != 0);

	while (Count != 0)
	{

		Count--;

		Transmit(TransmitChar[Count] + ASCII_CONVERSION);

	}

	//
	// Transmit a tab space
	//

	Transmit(ASCII_TAB);
}

void Transmit(unsigned char Value)
{

	while (!(UCSRA & (1 << UDRE)));

	UDR = Value;

}
