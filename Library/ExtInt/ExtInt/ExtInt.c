/*
 * ExtInt.c
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


#include "ExtInt.h"

#include "Trace.h"

//
// Global Definitions
//

bool gIsExtIntInitialized = FALSE;

PEXTINT_CALLBACK gExtIntCallback[MAX_EXTINTS] = {NULL, NULL, NULL};

PVOID gExtIntCallbackContext[MAX_EXTINTS] = {NULL, NULL, NULL};

//
// Public Functions
//

void
InitExtInt()
{
	cli();
	
	//
	// Set Control Register to 0
	//
	
	EICRA = 0;
	
	//
	// Set Interrupt Mask Register
	//
	
	EIMSK = 0;
	
	//
	// Set Interrupt Flag Register to zero
	//
	
	EIFR = 0x7;
	
	//
	// Set Pin Change Interrupt Control Register
	//
	
	PCICR = 0;
	
	//
	// Set Pin Change Interrupt Flag Register
	//
	
	PCIFR = 0xF;
	
	//
	// Set Pin Change Interrupt Mask Registers
	//
	
	PCMSK0 = 0x0;
	
	PCMSK1 = 0x0;
	
	PCMSK2 = 0x0;
	
	PCMSK3 = 0x0;
	
	//
	// Enable global interrupts
	//
	
	sei();
	
	gIsExtIntInitialized = TRUE;
	
}

bool
RegisterExtInterrupt(EXTINT_NUM ExtIntNum, EXTINT_POLARITY Polarity, PEXTINT_CALLBACK Callback, PVOID CallbackContext, bool EnableInternalPullup)
{
	bool Status = FALSE;

	if (!gIsExtIntInitialized)
	{
		SendTrace(ERROR_LEVEL, "External Interrupt Not Initialized\r\n");
		
		goto Exit;
	}
	
	if (Callback == NULL)
	{
		SendTrace(ERROR_LEVEL, "Invalid Callback Context\r\n");
		
		goto Exit;
	}
	
	if (gExtIntCallback[ExtIntNum] != NULL)
	{
		SendTrace(ERROR_LEVEL, "Interrupt Already Registered\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Ext Int Num = %d\r\n", ExtIntNum);
	
	//
	// Initialize callback function and callback context
	//
	
	gExtIntCallback[ExtIntNum] = Callback;
	
	gExtIntCallbackContext[ExtIntNum] = CallbackContext;
	
	//
	// Initialize External Interrupt Control Register
	//
	
	EICRA |= (Polarity << (ExtIntNum * 2));
	
	//
	// Initialize External Interrupt Mask Register
	//
	
	EIMSK |= (0x1 << ExtIntNum);
	
	//
	// Clear the Interrupt Flag
	//
	
	EIFR |= (0x1 << ExtIntNum);
	
	//
	// Make the port pin as input and enable pull up.
	//
	
	switch(ExtIntNum)
	{
		case EXTINT_INT0:
		{
			DDRD &= ~(0x1 << PORTD2);
			
			if (EnableInternalPullup)
			{
				PORTD |= (0x1 << PORTD2);
			}
			
			break;
		}
		case EXTINT_INT1:
		{
			DDRD &= ~(0x1 << PORTD3);
			
			if (EnableInternalPullup)
			{
				PORTD |= (0x1 << PORTD3);
			}
			
			break;
		}
		case EXTINT_INT2:
		{
			DDRD &= ~(0x1 << PORTB2);
			
			if (EnableInternalPullup)
			{
				PORTB |= (0x1 << PORTB2);
			}
			
			break;
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
UnRegisterExtInterrupt(EXTINT_NUM ExtIntNum)
{
	bool Status = FALSE;

	if (!gIsExtIntInitialized)
	{
		SendTrace(ERROR_LEVEL, "External Interrupt Not Initialized\r\n");
		
		goto Exit;
	}
	
	//
	// Set callback and callback context to NULL
	//
	
	gExtIntCallback[ExtIntNum] = NULL;
	
	gExtIntCallbackContext[ExtIntNum] = NULL;
	
	//
	// Mask the interrupts
	//
	
	EIMSK &= ~(0x1 << ExtIntNum);

	//
	// Set pin mode to output
	//
	
	switch(ExtIntNum)
	{
		case EXTINT_INT0:
		{
			DDRD |= (0x1 << PORTD2);
			
			break;
		}
		case EXTINT_INT1:
		{
			DDRD |= (0x1 << PORTD3);
			
			break;
		}
		case EXTINT_INT2:
		{
			DDRD |= (0x1 << PORTB2);
			
			break;
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
MaskExtInterrupt(EXTINT_NUM ExtIntNum)
{
	bool Status = FALSE;

	if (!gIsExtIntInitialized)
	{
		SendTrace(ERROR_LEVEL, "External Interrupt Not Initialized\r\n");
		
		goto Exit;
	}
	
	//
	// Mask the interrupts
	//
	
	EIMSK &= ~(0x1 << ExtIntNum);
	
	Status = TRUE;
	
	Exit:

	return Status;

}

bool
UnMaskExtInterrupt(EXTINT_NUM ExtIntNum)
{
	bool Status = FALSE;

	if (!gIsExtIntInitialized)
	{
		SendTrace(ERROR_LEVEL, "External Interrupt Not Initialized\r\n");
		
		goto Exit;
	}
	
	//
	// UnMask the interrupts
	//
	
	EIMSK |= (0x1 << ExtIntNum);
	
	Status = TRUE;
	
	Exit:

	return Status;

}

ISR(INT0_vect)
{
	//
	// Disable Interrupt while we process this.
	//
	
	cli();
	
	if (gExtIntCallback[0] != NULL)
	{
		(*gExtIntCallback[0])(gExtIntCallbackContext[0]);
	}
	
	//
	// Enable Interrupt
	//
	
	sei();
}

ISR(INT1_vect)
{
	//
	// Disable Interrupt while we process this.
	//
	
	cli();
	
	if (gExtIntCallback[1] != NULL)
	{
		(*gExtIntCallback[1])(gExtIntCallbackContext[1]);
	}
	
	//
	// Enable Interrupt
	//
	
	sei();
}

ISR(INT2_vect)
{
	//
	// Disable Interrupt while we process this.
	//
	
	cli();
	
	if (gExtIntCallback[2] != NULL)
	{
		(*gExtIntCallback[2])(gExtIntCallbackContext[2]);
	}
	
	//
	// Enable Interrupt
	//
	
	sei();
}