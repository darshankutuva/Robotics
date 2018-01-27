/*
 * PWM.c
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

#include "Common.h"

#include "PWM.h"

#include "Trace.h"

#define PWM_PULSE_MIN		224

unsigned char PwmChannelPulse[PWM_MAX_CHANNELS] = {0, 0, 0, 0};

bool gPWMInitialized = FALSE;

bool
InitPWM()
{	
	if (gPWMInitialized)
	{
		SendTrace(INFO_LEVEL,"PWM Already Initialized\r\n");
		
		return FALSE;
	}
	
	DDRD |= (0x1 << PD4) | (0x1 << PD5) | (0x1 << PD6) | (0x1 << PD7);
	
	//
	// Set Phase Correct PWM with TOP = 0xFF - (WGM2 = 0, WGM1 = 0, WGM0 = 1)
	// Clear OCOA and OCOB when up-counting and Set OC0A and OC0B on down counting.
	//
	
	TCCR1A = ((0x1 << COM1A0) | (0x1 << COM1A1) | (0x1 << COM1B0) | (0x1 << COM1B1) | (0x1 << WGM10));
	
	TCCR2A = ((0x1 << COM2A0) | (0x1 << COM2A1) | (0x1 << COM2B0) | (0x1 << COM2B1) | (0x1 << WGM20));
	
	//
	// Set Clock Pre-scaling to 256.
	//
	
	TCCR1B = (0x1 << CS12);
	
	TCCR2B = (0x1 << CS21) | (0x1 << CS22);
	
	//
	// Enable Compare Match Interrupts.
	//
	
	TIMSK1 = (0x1 << TOIE1);
	
	TIMSK2 = (0x1 << TOIE2);
	
	//
	// Set Timer Counter Register to 0.
	//
	
	TCNT1 = 0;
	
	TCNT2 = 0;
	
	gPWMInitialized = TRUE;

	return TRUE;
}

void
TurnOffPWM()
{
	TCCR1B = 0;
	
	TCCR2B = 0;
}

bool
PWMChangePulseDuration(PWM_CHANNEL PwmChannel, unsigned char Pulse)
{
	bool Status = FALSE;
	
	if (!gPWMInitialized)
	{
		goto Exit;	
	}
	
	SendTrace(INFO_LEVEL, "Channel = %d - Pulse = %d\r\n", PwmChannel, Pulse);
	
	if (Pulse > PWM_PULSE_DIVS)
	{
		Pulse = PWM_PULSE_DIVS;
	}
	
	PwmChannelPulse[PwmChannel] = PWM_PULSE_MIN + PWM_PULSE_DIVS - Pulse;
	
	Status = TRUE;
	
Exit:
	
	return Status;
}

ISR(TIMER1_OVF_vect)
{
	if (gPWMInitialized)
	{	
		
		OCR1A = PwmChannelPulse[PWM_CHANNEL_0];
		
		OCR1B = PwmChannelPulse[PWM_CHANNEL_1];
	}
	
	return;
}

ISR(TIMER2_OVF_vect)
{
	if (gPWMInitialized)
	{
		OCR2A = PwmChannelPulse[PWM_CHANNEL_2];
	
		OCR2B = PwmChannelPulse[PWM_CHANNEL_3];
	}
	
	return;
}