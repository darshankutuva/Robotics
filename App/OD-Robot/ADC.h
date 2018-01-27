#ifndef _ADC_H_
#define _ADC_H_

//
// We are two bytes for each channel selection
// Format: first five bits are zeros, start bit, signal/diff, D2, D1, D0, rest zeros
// Channel 1: 0000 0110 0000 0000
// Channel 2: 0000 0110 0100 0000
//

#define AD_CH0_SELECT	0x0600

#define AD_CH1_SELECT 	0x0640

#define CS_LOW()	(PORTB &= ~(1 << PB4))

#define CS_HIGH() 	(PORTB |= (1 << PB4))

typedef enum _AD_CHANNEL 
{

	AD_CHANNEL_0 = 0,

	AD_CHANNEL_1 = 1

} AD_CHANNEL;

void 
InitSPI();

unsigned int
ReadDataFromAD(AD_CHANNEL Channel);

unsigned char
SendReceiveSPI(unsigned char Value);

#endif
