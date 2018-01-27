/*
 *  RCCar.c
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

#include <avr/io.h>

#include "common.h"

#include "UART.h"

#include "Trace.h"

#include "XY-MK-5V-XY-FST.h"

#include "ADC.h"

//
// Constant Definition
//

#define NUM_INPUT_VALUES		2

#define TIMER_TOP				156

#define MIN_PULSE				3

#define CENTER_PULSE			6

#define MAX_PULSE				9

#define SERVO_PULSE_INTERVAL	170

#define MY_ADDRESS				0x1

#define DEST_ADDRESS			0xf

//
// Global Variable Definition
//

bool gPWMInitialized = FALSE;

unsigned char gCurrentPulse = CENTER_PULSE;

//
// Macro Definitions
//

#define DriveForward() (PORTC = (0x1 << PINC4) | (0x1 << PINC6))

#define DriveBackward() (PORTC = (0x1 << PINC5) | (0x1 << PINC7))

#define Stop() (PORTC = 0)

//
// Function Declaration
//

void
InitServoPWM();

bool
PerformHandshake();

//
// Function Definition
//

int main(void)
{	
	bool Status;
	
	unsigned int InputValues[NUM_INPUT_VALUES];
	
	unsigned long InputLength = NUM_INPUT_VALUES * sizeof(unsigned int);
	
	unsigned long VoltageLength = sizeof(unsigned int);
	
	ADC_INIT_PARAMS AdcParams;
	
	unsigned int Voltage;
	
	//
	// First Initialize Tracing
	//
	
	InitTrace(F_CPU, INFO_LEVEL | ERROR_LEVEL);
	
	SendTrace(INFO_LEVEL, "RC Car\r\n");
	
	SendTrace(INFO_LEVEL, "-----------------\r\n");
	
	//
	// Init PWM
	//
	
	InitServoPWM();
	
	//
	// Init ADC
	//
	
	//
	// Initialize ADC Params
	//
	
	AdcParams.Flags.IsLeftAdjustResult = 0;
	
	AdcParams.RefVoltSelect = AREF_VOLT;
	
	AdcParams.Prescalar = DIV_FACTOR_0;
	
	AdcParams.EnabledChannelsBitVector = (0x1 << ADC_CHANNEL_0);
	
	Status = InitADC(&AdcParams);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Init ADC Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Init Transceiver
	//
	
	Status = InitXymk5vXyfst(MY_ADDRESS, DEST_ADDRESS, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Init Transceiver Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Init H-Bridge
	// Make PC4, PC5, PC6 PC7 as output
	// 
	
	DDRC |= (0x1 << PINC4) | (0x1 << PINC5) | (0x1 << PINC6) | (0x1 << PINC7);
	
	//
	// Perform Handshake
	//
	
	SendTrace(INFO_LEVEL, "Performing Handshake\r\n");
	
	Status = PerformHandshake();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Handshake Failed\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Drive RC Car\r\n");
	
    while(1)
    {
		
		Status = Xymk5vXyfstReceive(&InputValues[0], &InputLength, GENERAL_MESSAGE);
        
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to receive command\r\n");
			
			continue;
		}
		
		if (InputLength != (NUM_INPUT_VALUES * sizeof(unsigned int)))
		{
			SendTrace(ERROR_LEVEL, "Invalid Length = %d\r\n", InputLength);
			
			continue;	
		}
		
		//
		// Program the H-Bridge to Drive Motor
		//
		
		if (InputValues[0] > ADC_CENTER)
		{	
			DriveForward();
		}
		else if (InputValues[0] < ADC_CENTER)
		{
			DriveBackward();
		}
		else
		{
			Stop();
		}
		
		//
		// Program Servo to turn in the selected direction
		//
		
		gCurrentPulse = (InputValues[1] / SERVO_PULSE_INTERVAL) + MIN_PULSE;
		
		SendTrace(INFO_LEVEL, "Current Pulse = %d\r\n", gCurrentPulse);
		
		//
		// Read Voltage
		//
		
		ADCRead(ADC_CHANNEL_0, &Voltage);
		
		ADCRead(ADC_CHANNEL_0, &Voltage);
		
		Xymk5vXyfstTransmit(&Voltage, VoltageLength, GENERAL_MESSAGE);
    }
	
Exit:
	
	return Status;
}

void
InitServoPWM()
{
	//
	// Set OC2A pin to output
	//
	
	DDRD |= (0x1 << PIND7);
	
	TCCR2A = (0x1 << COM2A0) | (0x1 << COM2A1) | (0x1 << WGM20);
	
	//
	// Set Clock Pre-scaling to 256.
	//
	
	TCCR2B = (0x1 << CS20) | (0x1 << CS21) | (0x1 << CS22) | (0x1 << WGM22);
	
	//
	// Set Timer Overflow Interrupt to 1
	//
	
	TIMSK2 = (0x1 << TOIE2);
	
	//
	// Set OCR2B to TOP value
	//
	
	OCR2B = TIMER_TOP;
	
	//
	// Set OCR2A and TCNT2 to 0
	//
	
	OCR2A = 0;
	
	TCNT2 = 0;
	
	gPWMInitialized = TRUE;
}

bool
PerformHandshake()
{
	bool Status = FALSE;
	
	unsigned long Length;
	
	unsigned short RxValue;
	
	Length = sizeof(unsigned short);
	
	//
	// Wait until handshake data arrives
	//
	
	Status = Xymk5vXyfstReceive(&RxValue, &Length, GENERAL_MESSAGE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "PerformHandshake - Receive Data Failed\r\n");
		
		goto Exit;
	}
	
	if (Length != sizeof(unsigned short))
	{
		SendTrace(ERROR_LEVEL, "PerformHandshake - Receive Data Failed - Received Length = %d\r\n", Length);
		
		Status = FALSE;
		
		goto Exit;
	}
	
	if (RxValue != ADC_MAX_VALUE)
	{
		SendTrace(ERROR_LEVEL, "PerformHandshake - Invalid Value - %d\r\n", RxValue);
		
		Status = FALSE;
		
		goto Exit;
	}
	
	RxValue++;
	
	Status = Xymk5vXyfstTransmit((PVOID)&RxValue, sizeof(unsigned short), GENERAL_MESSAGE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "PerformHandshake - Transmit Data Failed\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Handshake completed\r\n");
	
	Status = TRUE;
	
Exit:

	return Status;
}

ISR(TIMER2_OVF_vect)
{
	if (gPWMInitialized)
	{
		OCR2A = (TIMER_TOP - gCurrentPulse);
	}
	
	return;
}