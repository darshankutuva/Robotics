#define F_CPU 1000000UL

#include <avr/io.h>

#include "ADC.h"

void 
InitSPI()
{

	//
	// PB4 = SS (Slave Select)
	// PB5 = MOSI (Master Out Slave In)
	// PB6 = MISO (Master In Slave Out)
	// PB7 = SCK (Clock)
	// Make PB4, PB5, PB7 to output
	//

	DDRB |= ((1 << PB4) | (1 << PB5) | (1 << PB7));

	//
	// Make the CS pin high
	//

	CS_HIGH();

	//
	// Initialize SPCR register.
	// Set SPI as Master
	// Set the clock to 16 KHz
	//

	SPCR = ((1 << MSTR) | (1 << SPR0) | (1 << SPR1) | (1 << SPE));
	
		
}

unsigned int
ReadDataFromAD(AD_CHANNEL Channel)
{

	unsigned char Transmit1;

	unsigned char Transmit2;

	unsigned int Value = 0;

	unsigned char Receive1 = 0;

	unsigned char Receive2 = 0;


	//
	// First select the channel to read
	//

	switch (Channel)
	{
		case AD_CHANNEL_0:
		{

			Transmit1 = (unsigned char)(AD_CH0_SELECT >> 8);

			Transmit2 = (unsigned char)(AD_CH0_SELECT);
			
			break;

		}
		case AD_CHANNEL_1:
		{

			Transmit1 = (unsigned char)(AD_CH1_SELECT >> 8);

			Transmit2 = (unsigned char)(AD_CH1_SELECT);
			
			break;

		}
		default:
		{
			goto Exit; 
		}
	}

	//
	// Now lower the CS pin
	//

	CS_LOW();

	//
	// Send the first byte.
	//

	SendReceiveSPI(Transmit1);

	//
	// Now Send Second Byte.
	//

	Receive2 = SendReceiveSPI(Transmit2);

	Receive2 &= 0x0F;

	//
	// Now transmit 0xFF don't care bits.
	//

	Receive1 = SendReceiveSPI(0xFF);

	//
	// Make the chip select signal high.
	//

	CS_HIGH();

	//
	// Now assemble 12 bit value.
	//

	Value = Receive2;

	Value = Value << 8;

	Value |= Receive1;

Exit:

	return Value;

}

unsigned char
SendReceiveSPI(unsigned char Value)
{

	SPDR = Value;
	
	while (!(SPSR & (1 << SPIF)));
	
	return SPDR;	
}
