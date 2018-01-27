/*
 * Quadcopter.c
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


#include "Quadcopter.h"

//
// Constant Definition
//

#define MAX_WATCH_DOG_TIMER_TRIGGERS			64

//
// Macro Definition
//

#define StartWatchDogTimer()					\
{												\
	TCCR0B = (0x1 << CS02) | (0x1 << CS00);		\
												\
	TCNT0 = 0;									\
												\
	gIsSignalLost = FALSE;						\
												\
	gWatchDogTimerTriggerCount = 0;				\
}

#define StopWatchDogTimer()						\
{												\
	TCCR0B = 0x0;								\
}

//
// Global Variable Definition
//

unsigned char gWatchDogTimerTriggerCount = 0;

bool gIsSignalLost = FALSE;

int main(void)
{
	bool Status = FALSE;
	
	//
	// First Initialize Tracing
	//
	
	InitTrace(F_CPU, INFO_LEVEL | ERROR_LEVEL);
	
	SendTrace(INFO_LEVEL, "Quadcopter\r\n");
	
	SendTrace(INFO_LEVEL, "-----------------\r\n");
	
	//
	// Initialize TWI
	//

	if(!InitTWI(TWI_MASTER, NULL, NULL))
	{
		goto Exit;
	}
	
	//
	// Initialize Interrupts
	//
	
	InitExtInt();
	
	//
	// Now Initialize Transmitter
	//
	
	Status = InitTransmitter();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - Transmitter Initialization Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Initialize Accelerometer
	//
	
	Status = InitializeAccelerometer();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - Accelerometer Initialization Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Initialize Magnetometer
	//
	
	Status = InitHMC5883LMagnetometer(CONTINUOUS_MEASUREMENT_MODE, NORMAL, FIFTEEN_HZ, EIGHT_SAMPLES, THOUSAND_AND_NINTY);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - Magnetometer Initialization Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Initialize GPS
	//
	
	Status = InitializeGPS();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - GPS Initialization Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Perform Handshake
	//
	
	Status = PerformHandshake();
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "main - Handshake Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Init PWM
	//
	
	InitPWM();
	
	//
	// Initialize Watch Dog Timer
	//
	
	InitializeWatchDogTimer();
	
    //
	// Now Fly
	//
	
	FlyCopter();
	
Exit:

	return Status;
}

bool
InitTransmitter()
{
		
	TRANSCEIVER_INIT_PARAMS InitParams;
	
		
	//
	// Initialize Mode
	//
		
	InitParams.Mode = RECEIVER;
		
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
	
	InitParams.RxDRCallbackContext	= NULL;
		
	InitParams.TxDSCallbackContext	= NULL;
		
	//
	// Call initialization routine
	//
		
	return InitTransceiver(&InitParams);
}

bool
PerformHandshake()
{
	bool Status = FALSE;
	
	PAYLOAD_LENGTH Length;
	
	unsigned int Value;
	
	//
	// Write handshake acknowledgment data.
	//
	
	Length.AsUINT8 = 0;
	
	Length.PayloadLength = sizeof(unsigned int);
	
	Value = ADC_MAX_VALUE + 1;
	
	TCTransmitAckPayload(&Value, &Length);
	
	//
	// Start Watch Dog Timer
	//
	
	StartWatchDogTimer();
	
	//
	// Wait until acknowledgment data arrives
	//
	
	while(!TCIsReceivePayloadAvailable())
	{
		if (gIsSignalLost)
		{
			SendTrace(ERROR_LEVEL, "Signal Lost.\r\n");
			
			PerformEmergencyLanding();
			
			goto Exit;
		}
	}
	
	//
	// Stop Watch Dog Timer
	//
	
	StopWatchDogTimer();
		
	//
	// Get the received payload length
	//
		
	TCGetReceivedPayloadLength(&Length);
		
	if (Length.PayloadLength != sizeof(unsigned int))
	{
		SendTrace(ERROR_LEVEL, "PerformHandshake - Invalid Data Length, Length = %d\r\n", Length.PayloadLength);
			
		Status = FALSE;
			
		goto Exit;
	}
		
	//
	// Now receive the data
	//
	
	Value = 0;	
	
	//Length.PayloadLength = sizeof(unsigned int);
	
	Status = TCReceivePayload(&Value, &Length);
		
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "PerformHandshake - Receive Data Failed\r\n");
			
		goto Exit;
	}
	
	if (Value != ADC_MAX_VALUE)
	{
		SendTrace(ERROR_LEVEL, "PerformHandshake - Received value not equal to ADC MAX VALUE, Value = %d\r\n", Value);
		
		Status = FALSE;
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Handshake completed\r\n");
	
	Status = TRUE;
	
Exit:

	return Status;	
}

bool
InitializeGPS()
{
	return TRUE;
}

void
InitializeWatchDogTimer()
{
	//
	// Initialize Timer Counter 0 as watch dog timer.
	//
	
	//
	// Set Timer/Counter Control Register A to normal mode of operation.
	//
	
	TCCR0A = 0x0;
	
	//
	// Set Timer/Counter Control Register B to have a clock with pre-scalar = 1024.
	// The timer will expire every 32.64 ms.
	//
	
	TCCR0B = 0;
	
	//
	// Set Timer/Counter Register to 0xFF
	//
	
	TCNT0 = 0x0;
	
	//
	// Set OCR0A and OCR0B to zero
	//
	
	OCR0A = 0;
	
	OCR0B = 0;
	
	//
	// Enable Timer/Counter0 Overflow Interrupt
	//
	
	TIMSK0 = 0x1;
}

void
FlyCopter()
{
	bool Status; 
	
	PAYLOAD_LENGTH Length;
	
	unsigned int Value[NUM_INPUT_VALUES * 4];
	
	while(1)
	{	
		SendCompassHeading();
		
		//
		// Start Watch Dog Timer
		//
		
		StartWatchDogTimer();
		
		//
		// Wait until acknowledgment data arrives
		//
		
		while(!TCIsReceivePayloadAvailable())
		{
			if (gIsSignalLost)
			{
				SendTrace(ERROR_LEVEL, "Signal Lost.\r\n");
				
				PerformEmergencyLanding();
				
				goto Exit;
			}
		}
		
		//
		// Stop Watch Dog Timer
		//
		
		StopWatchDogTimer();
		
		//
		// Get the received payload length
		//
		
		TCGetReceivedPayloadLength(&Length);
		
		if ((Length.PayloadLength % (NUM_INPUT_VALUES * sizeof(unsigned int))) != 0)
		{
			SendTrace(ERROR_LEVEL, "Invalid Data Length - %d.\r\n", Length.PayloadLength);
			
			TCReceivePayload(&Value[0], &Length);
			
			continue;
		}
		
		SendTrace(INFO_LEVEL, "Length = %d\r\n", Length.AsUINT8);
		
		//
		// Now receive the data
		//
		
		Status = TCReceivePayload(&Value[0], &Length);
		
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "FlyCopter - Receive Data Failed\r\n");
			
			continue;
		}
		
		SendTrace(INFO_LEVEL, "Throttle = %d, XAxis = %d, YAxis = %d\r\n", Value[0], Value[1], Value[2]);
	
		CalculatePulse(Value[0], Value[1], Value[2]);
	}
	
Exit:

	return;
}

bool
SendCompassHeading()
{
	bool Status;
	
	ACC_TILT_DATA_XYZ AccTiltData;
	
	MAGNETOMETER_DATA MagData;
	
	PAYLOAD_LENGTH Length;
	
	double CompassHeading[2];
	
	//
	// We are transmitting Compass Heading as ACK Payload.
	// So we need to match the payload size of the input payload.
	// Compass Heading has 4 bytes. Input Payload has 8 bytes. So
	// pad Compass Heading payload with 4 bytes of 0's.
	//
	
	CompassHeading[1] = 0;
	
	Status = ReadAccelerometerData(&AccTiltData);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL,"Failed to read Accelerometer Data.\r\n");
		
		goto Exit;
	}
	
	Status = ReadHMC5883LContinuousMeasurementData(&MagData);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL,"Failed to read Magnetometer Data.\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Mag Data Read.\r\n");
	
	Status = CalculateCompassHeading(&MagData, AccTiltData.TiltX, AccTiltData.TiltY, DEGREES, &CompassHeading[0]);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL,"Failed to calculate compass heading.\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Compass Heading Calculated.\r\n");
	
	Length.AsUINT8 = 0;
	
	Length.PayloadLength = sizeof(double) * 2;
	
	Status = TCTransmitAckPayload((PVOID)&CompassHeading[0], &Length);
	
	SendTrace(INFO_LEVEL, "Ack Payload Transmitted.\r\n");
	
Exit:

	return Status;
}

ISR(TIMER0_OVF_vect)
{	
	if (gWatchDogTimerTriggerCount == MAX_WATCH_DOG_TIMER_TRIGGERS)
	{
		gIsSignalLost = TRUE;
		
		StopWatchDogTimer();
	}
	else
	{
		gWatchDogTimerTriggerCount++;
	}
}

