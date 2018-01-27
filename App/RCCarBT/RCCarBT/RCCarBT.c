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

//
// Constant Definitions
//

#define TIMER_TOP				0xFF

#define MIN_PULSE				12

#define CENTER_PULSE			23

#define MAX_PULSE				34

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

#define TurnLeft() ((gCurrentPulse < MAX_PULSE) ? (gCurrentPulse++) : (gCurrentPulse = MAX_PULSE))

#define TurnRight() ((gCurrentPulse > MIN_PULSE) ? (gCurrentPulse--) : (gCurrentPulse = MIN_PULSE))

#define TurnCenter() (gCurrentPulse = CENTER_PULSE)

//
// Function Declaration
//

void
InitServoPWM();

//
// Function Definition
//

int main(void)
{
	bool Status = TRUE;
	
	unsigned long Length;
	
	unsigned char Command;
	
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
	// Enable Interrupt
	//
	
	sei();
	
	//
	// Init UART 01
	//
	
	Status = InitUARTSync(UART_ONE, BR_9600, NORMAL_MODE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "UART 01 Initialization failed\r\n");
		
		goto Exit;
	}
	
	//
	// Init H-Bridge
	// Make PC4, PC5, PC6 PC7 as output
	//
	
	DDRC |= (0x1 << PINC4) | (0x1 << PINC5) | (0x1 << PINC6) | (0x1 << PINC7);
	
	SendTrace(INFO_LEVEL, "Drive RC Car\r\n");
	
	while(1)
	{
		Length = UARTReceiveSync(UART_ONE, &Command, sizeof(unsigned char));
		
		if (Length != sizeof(unsigned char))
		{
			SendTrace(ERROR_LEVEL, "Invalid Command Length\r\n");
			
			continue;
		}
		
		SendTrace(INFO_LEVEL, "Command = %d\r\n", Command);
		
		switch (Command)
		{
			case 1:
			{
				TurnCenter();
				
				break;
			}
			case 2:
			{
				TurnRight();
				
				break;
			}
			case 3:
			{
				TurnLeft();
				
				break;
			}
			case 4:
			{
				DriveForward();
				
				break;
			}
			case 5:
			{
				DriveBackward();
				
				break;
			}
			case 6:
			{
				Stop();
				
				break;
			}
			default:
			{
				SendTrace(ERROR_LEVEL, "Invalid Command\r\n");
				
				break;
			}
		}
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
	// Set Clock Pre-scaling to 1024.
	//
	
	TCCR2B = (0x1 << CS21) | (0x1 << CS22);
	
	//
	// Set Timer Overflow Interrupt to 1
	//
	
	TIMSK2 = (0x1 << TOIE2);
	
	//
	// Set OCR2A to TOP value
	//
	
	OCR2A = (unsigned char) (TIMER_TOP - gCurrentPulse);
	
	//
	// Set TCNT2 to 0
	//
	
	TCNT2 = 0;
	
	gPWMInitialized = TRUE;
}

ISR(TIMER2_OVF_vect)
{	
	if (gPWMInitialized)
	{
		OCR2A = (unsigned char) (TIMER_TOP - gCurrentPulse);
	}
	
	return;
}