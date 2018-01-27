/*
 * LCDTest.cpp
 *
 * Author: Shanthanand R Kutuva
 */ 


#include "Common.h"

#include "LCD.h"

#include "Trace.h"

#include "TWI.h"

#define LCD_ADDRESS		0x27

#define MAX_CHARACTERS  0x10

#define MAX_LINES		0x2

int main(void)
{
	bool Status;
	
	LCD_INIT_PARAMS InitParams;
	
	InitTrace(F_CPU, (ERROR_LEVEL | INFO_LEVEL));
	
	SendTrace(INFO_LEVEL,"LCD Test\r\n");
	
	SendTrace(INFO_LEVEL,"---------\r\n");
	
	//
	// Initialize Init Params Structure
	//
	
	InitParams.Flags.AsUINT16 = 0;
	
	InitParams.Flags.Is5X10Font = 0;
	
	InitParams.Flags.IsBackLightOn = 1;
	
	InitParams.Flags.IsCursorBlinkOn = 1;
	
	InitParams.Flags.IsCursorOn = 1;
	
	InitParams.Flags.IsDisplayOn = 1;
	
	InitParams.Flags.IsDisplayShift = 1;
	
	InitParams.Flags.IsIncrementPos = 0;
	
	InitParams.Flags.IsAutoScroll = 0;
	
	//
	// Set LCD Address
	//
	
	InitParams.SlaveAddress.AsUINT8 = 0;
	
	InitParams.SlaveAddress.Address = LCD_ADDRESS;
	
	//
	// Set Max Characters per line and number of lines
	//
	
	InitParams.MaxCharacters = MAX_CHARACTERS;
	
	InitParams.MaxLines = MAX_LINES;
	
	//
	// Initialize TWI
	//

	Status = InitTWI(TWI_MASTER, NULL, NULL);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL,"InitLCD - TWI Initialization Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Initialize LCD
	//

	InitLCD(&InitParams);
	
	//
	// Set Cursor Position
	//
	
	LCDSetCursorPosition(0, 0);
	
	//
	// Display String
	//
	
	LCDPrint("Welcome To Robot World!! Hello");
	
	//
	// Set Cursor Position
	//
		
	//LCDSetCursorPosition(1, 0);
	
	//
	// Display String
	//
		
	//LCDPrint("Hello Pinchu!!");
	
Exit:

	return Status;
}