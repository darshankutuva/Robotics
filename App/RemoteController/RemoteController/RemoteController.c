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

double gCompassHeading;

int main(void)
{
	bool Status = FALSE;
	
	//
	// First Initialize Tracing
	//
	
	InitTrace(F_CPU, ERROR_LEVEL | INFO_LEVEL);
	
	SendTrace(INFO_LEVEL, "Remote Controller\r\n");
	
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
	
	//
	// Now Initialize Transmitter
	//
	
	InitTransmitter();
	
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
	// Now wait for the throttle to be set to low (for flying) or high (for programming)
	//
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Fly: Set TH Low");
	
	LCDSetCursorPosition(1, 0);
	
	LCDPrint("Prg: Set TH High");
	
	WaitForThrottle();
	
	//
	// Now wait for Enter to be pressed to connect with the Copter
	//
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Press Enter To");
	
	LCDSetCursorPosition(1, 0);
	
	LCDPrint("Connect");
	
	SendTrace(INFO_LEVEL, "Flag = %d\r\n", gIsEnterPressed);
	
	WaitForEvent(gIsEnterPressed);
	
	//
	// Now connect to Copter
	//
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Connecting...");
	
	Status = ConnectToCopter();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - Failed to Connect\r\n");
		
		LCDClearDisplay();
		
		LCDSetCursorPosition(0, 0);
		
		LCDPrint("Operation Failed");
		
		goto Exit;
	}
	
	//
	// Now fly copter
	//
    
	_delay_ms(1000);
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	LCDPrint("Connected");
	
	FlyCopter();     
	
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
	ADC_INIT_PARAMS AdcParams;
	
	//
	// Register INT0 Interrupt
	//
	
	RegisterExtInterrupt(EXTINT_INT0, FALLING_EDGE_TRIGGERED, &RCADCIntCallback, NULL, TRUE);
	
	//
	// Initialize ADC Params
	//
	
	AdcParams.Flags.IsLeftAdjustResult = 0;
	
	AdcParams.RefVoltSelect = AREF_VOLT;
	
	AdcParams.Prescalar = DIV_FACTOR_0;
	
	AdcParams.EnabledChannelsBitVector = (0x1 << ADC_CHANNEL_0) | (0x1 << ADC_CHANNEL_1) | (0x1 << ADC_CHANNEL_2);
	
	return InitADC(&AdcParams);
}

bool
InitTransmitter()
{
	TRANSCEIVER_INIT_PARAMS InitParams;
	
	//
	// Initialize Mode
	//
	
	InitParams.Mode = TRANSMITTER;
	
	//
	// Initialize Auto Acknowledgment
	//
	
	InitParams.IsAutoAcknowledgeEnabled = TRUE;
	
	//
	// Initialize MY Address
	//
	
	InitParams.MyAddress.AsULongLong = 0x0;
	
	InitParams.MyAddress.Address = MY_ADDRESS;
	
	//
	// Initialize Destination Address
	//
	
	InitParams.DestAddress.AsULongLong = 0x0;
	
	InitParams.DestAddress.Address = DEST_ADDRESS;
	
	//
	// Initialize Retry parameters
	//
	
	InitParams.AutoRetryCount = ARC_15;
	
	InitParams.AutoRetryDelay = ARD_4000_MS;
	
	//
	// Initialize RF Channel
	//
	
	InitParams.RFChannel.AsUINT8 = 0;
	
	InitParams.RFChannel.RFCH = RF_CH;
	
	//
	// Register Interrupt Callbacks
	//
	
	InitParams.MaxRTCallback = NULL;
	
	InitParams.RxDRCallback = NULL;
	
	InitParams.TxDSCallback = NULL;
	
	//
	// Initialize call back context
	//
	
	InitParams.MaxRTCallbackContext = NULL;
	
	InitParams.TxDSCallbackContext	= NULL;
	
	InitParams.RxDRCallbackContext	= NULL;
	
	//
	// Call initialization routine
	//
	
	return InitTransceiver(&InitParams);
}

void
WaitForThrottle()
{
	unsigned int Value = 0;
	
	do
	{
		ADCRead(ADC_CHANNEL_0, &Value);
		
		ADCRead(ADC_CHANNEL_0, &Value);
	
	} while (Value != 0 && Value != (ADC_MAX_VALUE - 1));
}

bool
ConnectToCopter()
{
	bool Status;
	
	unsigned int Value = ADC_MAX_VALUE;
	
	PAYLOAD_LENGTH Length;
	
	//
	// Perform handshake with the Copter.
	//
	
	
	Length.AsUINT8 = 0;
		
	Length.PayloadLength = sizeof(unsigned int);
		
	//
	// Transmit Data
	//
	
	SendTrace(ERROR_LEVEL, "ConnectToCopter - Transmit Data\r\n");
				
	Status = TCTransmitPayload((PVOID) &Value, &Length);
		
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "ConnectToCopter - Transmit Data Failed\r\n");
			
		goto Exit;
	}
		
	//
	// Wait until acknowledgment data arrives
	//
	
	SendTrace(ERROR_LEVEL, "ConnectToCopter - Wait For Data\r\n");
	
	Status = VerifyAckData();
	
Exit:
	
	return Status;
}

bool
VerifyAckData()
{
	bool Status;
	
	unsigned int RxValue = 0;
	
	PAYLOAD_LENGTH Length;
	
	//WaitForEvent(gIsDataReceived);
	
	while(!TCIsReceivePayloadAvailable());
	
	//
	// Get the received payload length
	//
	
	TCGetReceivedPayloadLength(&Length);
	
	if (Length.PayloadLength != sizeof(unsigned int))
	{
		SendTrace(ERROR_LEVEL, "ConnectToCopter - Invalid Data Length\r\n");
		
		Status = FALSE;
		
		goto Exit;
	}
	
	//
	// Now receive the data
	//
	
	Status = TCReceivePayload(&RxValue, &Length);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "ConnectToCopter - Receive Data Failed\r\n");
		
		goto Exit;
	}
	
	if (RxValue != ADC_MAX_VALUE + 1)
	{
		SendTrace(ERROR_LEVEL, "ConnectToCopter - Invalid Acknowledgment\r\n");
		
		Status = FALSE;
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "ConnectToCopter - Ack Value = %d\r\n", RxValue);
	
	Status = TRUE;
	
Exit:

	return Status;
}

void
FlyCopter()
{
	PAYLOAD_LENGTH Length;
	
	bool Status;
	
	unsigned int InputValue[NUM_INPUT_VALUES];
	
	double CompassHeadingBuffer[8];
	
	Length.AsUINT8 = 0;
	
	Length.PayloadLength = NUM_INPUT_VALUES * sizeof(unsigned int);
	
	//
	// Input Value 3 is just padding. So set it to 0.
	//
	
	InputValue[3] = 0;
	
	while (1)
	{
		
		//
		// Read Throttle Value
		//
			
		ADCRead(ADC_CHANNEL_0, &InputValue[0]);
			
		ADCRead(ADC_CHANNEL_0, &InputValue[0]);
		
		//
		// Read X-axis Value
		//
			
		ADCRead(ADC_CHANNEL_1, &InputValue[1]);
			
		ADCRead(ADC_CHANNEL_1, &InputValue[1]);
			
		//
		// Read Y-axis Value
		//
			
		ADCRead(ADC_CHANNEL_2, &InputValue[2]);
			
		ADCRead(ADC_CHANNEL_2, &InputValue[2]);
		
		//
		// Transmit Data
		//
		
		TCTransmitPayload((PVOID) &InputValue[0], &Length); 
		
		//
		// Check if we have got Mag and GPS data
		//
		
		if(TCIsReceivePayloadAvailable())
		{
			
			TCGetReceivedPayloadLength(&Length);
			
			if ((Length.PayloadLength % sizeof(double)) != 0)
			{
				SendTrace(ERROR_LEVEL, "ConnectToCopter - Invalid Data Length\r\n");
				
				TCReceivePayload(&CompassHeadingBuffer[0], &Length);
				
				continue;
			}
			
			//
			// Now receive the data
			//
			
			Status = TCReceivePayload(&CompassHeadingBuffer[0], &Length);
			
			if (!Status)
			{
				SendTrace(ERROR_LEVEL, "ConnectToCopter - Receive Data Failed\r\n");
				
				continue;
			}
			
			gCompassHeading = CompassHeadingBuffer[0];
			
			UpdateLCD();
		}
	}
}

void
UpdateLCD()
{
	char Direction[3];
	
	LCDClearDisplay();
	
	LCDSetCursorPosition(0, 0);
	
	GetDirection(gCompassHeading, &Direction[0], 3);
	
	SendTrace(INFO_LEVEL, "Compass Heading = %f.\r\n", gCompassHeading);
	
	LCDPrint("Direction");
	
	LCDSetCursorPosition(1, 0);
	
	LCDPrint(&Direction[0]);
}

bool
GetDirection(double CompassHeading, char *Direction, unsigned char Length)
{
	if (Length < 3 || CompassHeading < 0 || CompassHeading > 360)
	{
		return FALSE;
	}
	
	if (CompassHeading >= 0 && CompassHeading <= 15)
	{
		strcpy(Direction, "N");
	}
	else if(CompassHeading > 15 && CompassHeading <= 75)
	{
		strcpy(Direction, "NE");
	}
	else if(CompassHeading > 75 && CompassHeading <= 105)
	{
		strcpy(Direction, "E");
	}
	else if(CompassHeading > 105 && CompassHeading <= 165)
	{
		strcpy(Direction, "SE");
	}
	else if(CompassHeading > 165 && CompassHeading <= 195)
	{
		strcpy(Direction, "S");
	}
	else if(CompassHeading > 195 && CompassHeading <= 255)
	{
		strcpy(Direction, "SW");
	}
	else if(CompassHeading > 255 && CompassHeading <= 285)
	{
		strcpy(Direction, "W");
	}
	else if(CompassHeading > 285 && CompassHeading <= 345)
	{
		strcpy(Direction, "NW");
	}
	else
	{
		strcpy(Direction, "N");
	}
	
	return TRUE;
}

void
RCADCIntCallback(PVOID CallbackContext)
{
	gIsEnterPressed = TRUE;
}

