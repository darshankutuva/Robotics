/*
 * LCD.c
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

#include "LCDInternal.h"

#include "Trace.h"

bool gLCDInitialized = FALSE;

bool gBackLight = FALSE;

bool gIsAutoScroll = FALSE;

unsigned char gCursorPosition = 0;

unsigned char gLineNumber = 0;

unsigned char gMaxCharacters = 0;

unsigned char gMaxLines = 0;

TWI_ADDRESS gSlaveAddress;

//
// Public Functions
//

bool
InitLCD(PLCD_INIT_PARAMS InitParams)
{
	bool IsTwoLineDisplay = FALSE;
		
	if (InitParams->MaxCharacters == 0 || InitParams->MaxLines == 0)
	{
		goto Exit;
	}
	
	//
	// Set Slave Address
	//
	
	gSlaveAddress.AsUINT8 = InitParams->SlaveAddress.AsUINT8;
	
	//
	// Clear Display
	//
		
	LCDClearDisplay();
	
	//
	// Set Function Set Control
	//
	
	gMaxLines = InitParams->MaxLines;
	
	gMaxCharacters = InitParams->MaxCharacters;
	
	if (gMaxLines > 1)
	{
		InitParams->Flags.Is5X10Font = 0;
		
		IsTwoLineDisplay = TRUE;
	}
	
	LCDSetFunctionControl(IsTwoLineDisplay, InitParams->Flags.Is5X10Font);
	
	//
	// Set Display ON/OFF Control
	//
	
	LCDSetDisplayControl(InitParams->Flags.IsDisplayOn, 
						 InitParams->Flags.IsCursorOn, 
						 InitParams->Flags.IsCursorBlinkOn,
						 InitParams->Flags.IsBackLightOn);
	
	//
	// Set Entry Mode
	//
	
	LCDSetEntryMode(InitParams->Flags.IsIncrementPos, InitParams->Flags.IsDisplayShift);
	
	//
	// Set Auto Scroll Flag
	//
	
	gIsAutoScroll = InitParams->Flags.IsAutoScroll;
	
	gLCDInitialized = TRUE;
	
Exit:

	return gLCDInitialized;

}

bool
LCDClearDisplay()
{	
	bool Status;
	
	Status = LCDTransmit(CLEAR_DISPLAY_CTRL, LCD_IR_REG, TRUE);
	
	LCDSetCursorToHomePos();

	return Status;
}

bool
LCDSetCursorToHomePos()
{
	bool Status;
	
	Status = LCDTransmit(RETURN_HOME_CTRL, LCD_IR_REG, TRUE);
	
	_delay_ms(2);

	return Status;
}

bool
LCDSetDisplayControl(bool IsDisplayOn, bool IsCursorOn, bool IsCursorBlinkOn, bool IsBackLightOn)
{
	bool Status = FALSE;
	
	unsigned char Value = DISPLAY_CTRL;
	
	//
	// Set Display On Flag
	//
	
	if (IsDisplayOn)
	{
		Value |= DISPLAY_ON;
	}
	
	//
	// Set Cursor On Flag
	//
	
	if (IsCursorOn)
	{
		Value |= CURSOR_ON;
	}
	
	//
	// Set Cursor Blink On Flag
	//
	
	if (IsCursorBlinkOn)
	{
		Value |= BLINK_ON;
	}
	
	//
	// Set Back Light On
	//
	
	gBackLight = IsBackLightOn;
	
	Status = LCDTransmit(Value, LCD_IR_REG, TRUE);

	return Status;
}

bool
LCDSetCursorPosition(unsigned char LineNumber, unsigned char Position)
{
	bool Status = FALSE;
	
	unsigned char Value = DDRAM_CTRL;
	
	if (!gLCDInitialized)
	{
		SendTrace(ERROR_LEVEL, "LCDSetCursorPosition - LCD Not Initialized\r\n");
		
		goto Exit;
	}
	
	if (LineNumber >= gMaxLines)
	{
		goto Exit;
	}
	
	if (Position >= MAX_BUFFER_SIZE)
	{
		goto Exit;
	}
	
	//
	// Set DDRAM Address
	//
	
	Value |= (LineNumber * DDRAM_ADDRESS_INTERLEAVING) + Position;
	
	Status = LCDTransmit(Value, LCD_IR_REG, TRUE);
	
	if (Status)
	{
		gLineNumber = LineNumber;
		
		gCursorPosition = Position;
	}
	
Exit:

	return Status;
}

bool
LCDPrint(char *String)
{
	bool Status = FALSE;
	
	unsigned long Len;
	
	unsigned long IndexI;
	
	if (!gLCDInitialized)
	{
		SendTrace(ERROR_LEVEL, "LCDPrint - LCD Not Initialized\r\n");
		
		goto Exit;
	}
	
	Len = strlen(String);
	
	//
	// Loop through each character and send it to Data Register
	//
	
	for(IndexI = 0; IndexI < Len; IndexI++)
	{
		if (gCursorPosition >= gMaxCharacters)
		{

			if (gIsAutoScroll)
			{
				//
				// If Auto scroll is enabled shit the display
				//
			
				LCDShiftCursorOrDisplay(TRUE, FALSE);
			}
			else
			{
				//
				// Do word wrap
				//
			
				gLineNumber = (gLineNumber + 1) % gMaxLines;
			
				gCursorPosition = 0;
				
				SendTrace(INFO_LEVEL, "Exceeded Max Chars - %d, %d\r\n", gLineNumber, gCursorPosition);
			
				LCDSetCursorPosition(gLineNumber, gCursorPosition);
			}
		}
		
		Status = LCDTransmit(*String, LCD_DR_REG, TRUE);
		
		if (!Status)
		{
			goto Exit;
		}
		
		gCursorPosition = (gCursorPosition + 1) % MAX_BUFFER_SIZE;
		
		String++;
	}
	
Exit:

	return Status;
}

//
// Private Functions
//

bool
LCDSetEntryMode(bool IsDisplayShift, bool IsIncrement)
{
	bool Status; 
	
	unsigned char Value = ENTRY_MODE_CTRL;
	
	//
	// Set Display Shift Flag
	//
	
	if (IsDisplayShift)
	{
		Value |= EM_DISPLAY_SHIFT;
	}
	
	//
	// Set Increment Position Flag
	//
	
	if (IsIncrement)
	{
		Value |= INCREMENT_POS;
	}
	
	Status = LCDTransmit(Value, LCD_IR_REG, TRUE);

	return Status;

}

bool
LCDShiftCursorOrDisplay(bool IsDisplayShift, bool IsIncrement)
{
	bool Status;
	
	unsigned char Value = CURSOR_DISPLAY_SHIFT_CTRL;
	
	//
	// Set Display Shift Flag
	//
	
	if (IsDisplayShift)
	{
		Value |= CD_DISPLAY_SHIFT;
	}
	
	//
	// Set Increment Flag
	//
	
	if (IsIncrement)
	{
		Value |= MOVE_RIGHT;
	}
	
	Status = LCDTransmit(Value, LCD_IR_REG, TRUE);

	return Status;
}

bool
LCDSetFunctionControl(bool IsTwoLineDisplay, bool Is5X10Font)
{
	bool Status;
	
	unsigned char Value = FUNC_SET_CTRL;
	
	//
	// Set Two Line Display Flag
	//
	
	if(IsTwoLineDisplay)
	{	
		Value |= TWO_LINES;
	}
	
	//
	// Set 5X10 Font Flag
	//
	
	if(Is5X10Font)
	{
		Value |= FONT_5X10;
	}
	
	Status = LCDTransmit(Value, LCD_IR_REG, TRUE);

	return Status;
}

bool
LCDTransmit(unsigned char Value, LCD_REG_SELECT RegSelect, bool IsEndOfTransfer)
{
	bool Status;
	
	LCD_TWI_DATA TwiData;
	
	
	TwiData.RegSelect = RegSelect;
	
	TwiData.BackLight = gBackLight;
	
	TwiData.ReadWrite = 0;
	
	//
	// We send upper nibble first and then the lower nibble of the Value
	// First four bits E, RS, BL, RW stays the same in both the transfers.
	//
	
	TwiData.Data = Value >> UPPER_NIBBLE_SHIFT;
	
	Status = LCDWrite(&TwiData, FALSE);
	
	if (!Status)
	{
		goto Exit;
	}
	
	//
	// Send the Lower Nibble
	//
	
	TwiData.Data = Value & LOWER_NIBBLE_MASK;
	
	Status = LCDWrite(&TwiData, IsEndOfTransfer);
	
	if (!Status)
	{
		goto Exit;
	}
	
Exit:
	
	return Status;
}

bool
LCDWrite(PLCD_TWI_DATA TwiData, bool IsEndOfTransfer)
{
	bool Status = FALSE;
	
	gSlaveAddress.RW_EnGenCall = 0;
	
	//
	// Send the value using TWI Transmit.
	//
	
	TwiData->Enable = 1;
	
	Status = TwiTransmit(&TwiData->AsUINT8, 1, &gSlaveAddress, FALSE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Transmit - Twi Transmit Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Now pulse the enable bit.
	//
	
	TwiData->Enable = 0;

	Status = TwiTransmit(&TwiData->AsUINT8, 1, &gSlaveAddress, IsEndOfTransfer);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Transmit - Twi Transmit Failed\r\n");
		
		goto Exit;
	}
	
	
	
Exit:

	return Status;
}