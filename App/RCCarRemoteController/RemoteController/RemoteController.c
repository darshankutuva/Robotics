/*
 * RemoteController.c
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

#include "RemoteController.h"

#include "ExtInt.h"

bool gIsEnterPressed = FALSE;

int main(void)
{
	bool Status = FALSE;
	
	//
	// First Initialize Tracing
	//
	
	InitTrace(F_CPU, ERROR_LEVEL | INFO_LEVEL);
	
	SendTrace(INFO_LEVEL, "RC Car Remote Controller\r\n");
	
	SendTrace(INFO_LEVEL, "-----------------\r\n");
	
	//
	// Initialize Interrupts
	//
	
	InitExtInt();
	
	//
	// Initialize TWI
	//
	
	Status = InitTWI(TWI_MASTER, NULL, NULL);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL,"main - TWI Initialization Failed\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Init TWI Completed\r\n");
	
	//
	// Initialize Display
	//
	
	Status = InitDisplay();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL,"main - Display Initialization Failed\r\n");
		
		goto Exit;
	}
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Welcome!!");
	
	_delay_ms(1000);
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Initializing...");
	
	SendTrace(INFO_LEVEL, "Init Display Completed\r\n");
	
	//
	// Initialize Input Control
	//
	
	Status = InitInputControl();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - Input Controller Initialization Failed\r\n");
		
		LCDClearDisplay();
		
		LCDSetCursorPosition(0, 0);
		
		LCDPrint("Operation Failed");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Init Input Control Completed\r\n");
	
	//
	// Init Transceiver
	//
	
	InitXymk5vXyfst(MY_ADDRESS, DEST_ADDRESS, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - Transmitter Initialization Failed\r\n");
		
		LCDClearDisplay();
		
		LCDSetCursorPosition(0, 0);
		
		LCDPrint("Operation Failed");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL,"Initialization Completed\r\n");
	
	//
	// Now wait for Enter to be pressed to connect with the RCCar
	//
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Press Enter To");
	
	LCDSetCursorPosition(1, 0);
	
	LCDPrint("Connect");
	
	SendTrace(INFO_LEVEL, "Flag = %d\r\n", gIsEnterPressed);
	
	WaitForEvent(gIsEnterPressed);
	
	//
	// Now connect to RC Car
	//
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Connecting...");
	
	Status = ConnectToRCCar();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - Failed to Connect\r\n");
		
		LCDClearDisplay();
		
		LCDSetCursorPosition(0, 0);
		
		LCDPrint("Operation Failed");
		
		goto Exit;
	}
	
	//
	// Now drive RCCar
	//
    
	_delay_ms(1000);
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Connected");
	
	DriveRCCar();     
	
	Status = TRUE;
	
Exit:

	return Status;
}

bool
InitDisplay()
{
	LCD_INIT_PARAMS InitParams;
	
	//
	// Initialize Init Params Structure
	//
	
	InitParams.Flags.AsUINT16 = 0;
	
	InitParams.Flags.Is5X10Font = 0;
	
	InitParams.Flags.IsBackLightOn = 1;
	
	InitParams.Flags.IsCursorBlinkOn = 0;
	
	InitParams.Flags.IsCursorOn = 0;
	
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
	// Initialize LCD
	//

	return InitLCD(&InitParams);
}

bool
InitInputControl()
{
	bool Status;
	
	ADC_INIT_PARAMS AdcParams;
	
	//
	// Register INT2 Interrupt
	//
	
	Status = RegisterExtInterrupt(EXTINT_INT2, FALLING_EDGE_TRIGGERED, &RCADCIntCallback, NULL, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to register external interrupts\r\n");
		
		goto Exit;
	}
	
	//
	// Initialize ADC Params
	//
	
	AdcParams.Flags.IsLeftAdjustResult = 0;
	
	AdcParams.RefVoltSelect = AREF_VOLT;
	
	AdcParams.Prescalar = DIV_FACTOR_0;
	
	AdcParams.EnabledChannelsBitVector = (0x1 << ADC_CHANNEL_0) | (0x1 << ADC_CHANNEL_1);
	
	Status = InitADC(&AdcParams);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to Init ADC\r\n");
	}
	
Exit:

	return Status;
}

bool
ConnectToRCCar()
{
	bool Status;
	
	unsigned short Value = ADC_MAX_VALUE;
	
	//
	// Perform handshake with the RCCar.
	//
		
	//
	// Transmit Data
	//
	
	SendTrace(INFO_LEVEL, "ConnectToRCCar - Transmit Data\r\n");
	
	Status = Xymk5vXyfstTransmit((PVOID)&Value, sizeof(unsigned short), GENERAL_MESSAGE);
		
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "ConnectToRCCar - Transmit Data Failed\r\n");
			
		goto Exit;
	}
		
	//
	// Wait until acknowledgment data arrives
	//
	
	SendTrace(ERROR_LEVEL, "ConnectToRCCar - Wait For Data\r\n");
	
	Status = VerifyAckData();
	
Exit:
	
	return Status;
}

bool
VerifyAckData()
{
	bool Status;
	
	unsigned short RxValue = 0;
	
	unsigned long Length = sizeof(unsigned short);
	
	SendTrace(INFO_LEVEL, "Verify Ack Data\r\n");
	
	//
	// Now receive the data
	//
	
	Status = Xymk5vXyfstReceive(&RxValue, &Length, GENERAL_MESSAGE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "VerifyAckData - Receive Data Failed\r\n");
		
		goto Exit;
	}
	
	if (Length != sizeof(unsigned short))
	{
		SendTrace(ERROR_LEVEL, "VerifyAckData - Receive Data Failed - Received Length = %d\r\n", Length);
		
		goto Exit;	
	}
	
	if (RxValue != ADC_MAX_VALUE + 1)
	{
		SendTrace(ERROR_LEVEL, "VerifyActData - Invalid Acknowledgment\r\n");
		
		Status = FALSE;
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "VerifyActData - Ack Value = %d\r\n", RxValue);
	
	Status = TRUE;
	
Exit:

	return Status;
}

void
DriveRCCar()
{
	unsigned long Length;
	
	bool Status;
	
	unsigned int InputValue[NUM_INPUT_VALUES];
	
	unsigned int Voltage;
	
	while (1)
	{
	
		//
		// Read X-axis Value
		//
			
		ADCRead(ADC_CHANNEL_0, &InputValue[0]);
			
		ADCRead(ADC_CHANNEL_0, &InputValue[0]);
			
		//
		// Read Y-axis Value
		//
			
		ADCRead(ADC_CHANNEL_1, &InputValue[1]);
			
		ADCRead(ADC_CHANNEL_1, &InputValue[1]);
		
		//
		// Transmit Data
		//
		
		Status = Xymk5vXyfstTransmit((PVOID) &InputValue[0], NUM_INPUT_VALUES * sizeof(unsigned int), GENERAL_MESSAGE); 
		
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to Transmit Data\r\n");
			
			continue;
		}
		
		//
		// Check if we have got Charging Voltage
		//
		
			
		//
		// Now receive the data
		//
			
		Length = sizeof(unsigned int);
			
		Status = Xymk5vXyfstReceive(&Voltage, &Length, GENERAL_MESSAGE);
		
		if (Length != sizeof(unsigned int))
		{	
			SendTrace(ERROR_LEVEL, "DriveRCCar - Received Length Invalid - Length = %d\r\n", Length);
			
			continue;	
		}
			
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "DriveRCCar - Receive Data Failed\r\n");
				
			continue;
		}
			
		UpdateLCD(Voltage);
	}
}

void
UpdateLCD(unsigned int Voltage)
{	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	SendTrace(INFO_LEVEL, "Voltage = %f.\r\n", Voltage);
	
	LCDPrint("Charging...");
	
	LCDSetCursorPosition(1, 0);
	
	//LCDPrint(&Direction[0]);
}

void
RCADCIntCallback(PVOID CallbackContext)
{
	gIsEnterPressed = TRUE;
}

