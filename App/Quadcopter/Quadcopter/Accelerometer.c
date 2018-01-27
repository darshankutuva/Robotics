/*
 * Accelerometer.c
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

#include "Accelerometer.h"

#include "Trace.h"

bool InitializeAccelerometer()
{
	
	INT_ENABLE_MAP_SOURCE IntEnable;
	
	INT_ENABLE_MAP_SOURCE IntMap;
	
	DATA_OFFSET DataOffset;
	
	FIFO_CTL FifoCtl;
	
	ACT_INACT_CTL ActInactCtl;
	
	bool Status = FALSE;
	
	//
	// Initialize Accelerometer
	//
	
	if(!InitADXL345DevAddress(TRUE))
	{	
		goto Exit;
	}
	
	//
	// Place Accelerometer in accelerometer in standby mode
	//
	
	if(!EnableDisableADXL345StandbyMode(TRUE))
	{
		goto Exit;
	}
	
	//
	// Program FIFO Control
	//
	
	FifoCtl.AsUINT8 = 0;
	
	FifoCtl.FIFOMode = BYPASS;
	
	FifoCtl.Samples = 0;
	
	if (!ProgramADXL345FifoControl(&FifoCtl))
	{
		goto Exit;
	}
	
	//
	// Program Activity Inactivity Detection
	//
	
	ActInactCtl.AsUINT8 = 0;
	
	ActInactCtl.ActXEnable = 1;
	
	ActInactCtl.ActYEnable = 1;
	
	ActInactCtl.ActZEnable = 1;
	
	if (!ProgramADXL345ActivityInactivityDetection(&ActInactCtl, 0x8, 0x8, 0xf))
	{
		goto Exit;
	}
	
	//
	// Program Data Format
	//
	
	//
	// Program Data Offset
	//
	
	DataOffset.OffsetX = -1;
	
	DataOffset.OffsetY = -4;
	
	DataOffset.OffsetZ = 6;
	
	if(!ProgramADXL345DataFormat(TWO_G, TRUE, &DataOffset, HUNDRED_HZ))
	{
		goto Exit;
	}
	
	//
	// Program Interrupts
	//
	
	IntEnable.AsUINT8 = 0;
	
	IntEnable.DataReady = 1;
	
	IntMap.AsUINT8 = 0;
	
	IntMap.DataReady = 1;
	
	if(!ProgramADXL345Interrupts(&IntEnable, &IntMap))
	{
		goto Exit;
	}
	
	//
	// Set Interrupt Mask in Atmel Atmega1284P.
	//
	// PCINT21 is PORT C - Pin 5
	//
	
	//
	// Set PORT C - Pin 5 as input
	//
	
	DDRC &= ~(1 << DDC5);
	
	//
	// Turn on the pull up resistor for PORT C - Pin 5
	//
	
	PORTC |= (1 << PORTC5);
	
	PCMSK2 = PCMSK2 | (0x1 << PCINT21);
	
	PCICR = PCICR | (0x1 << PCIE2);
	
	sei();
	
	//
	// Flush FIFO
	//
	
	if(!FlushADXL345Fifo())
	{
		goto Exit;
	}
	
	//
	// Program Accelerometer Power Mode
	//
	
	if(!ProgramADXL345PowerCtl(FULL_POWER, NO_WAKEUP))
	{
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Accelerometer Setup Complete\r\n");
	
	Status = TRUE;
	
Exit:

	return Status;
}

bool ReadAccelerometerData(PACC_TILT_DATA_XYZ AccTiltData)
{
	bool Status = FALSE;
	
	INT_ENABLE_MAP_SOURCE IntSource;
	
	if (!ReadADXL345InterruptSource(&IntSource))
	{
		goto Exit;
	}
		
	SendTrace(INFO_LEVEL, "Interrupt Source = 0x%x\r\n", IntSource.AsUINT8);
		
	if (IntSource.DataReady)
	{
		if(!ReadADXL345AccAndTiltData(DEGREES, AccTiltData))
		{
			goto Exit;
		}
					
		SendTrace(INFO_LEVEL, "X Axis Acc = %fg\t", AccTiltData->AccX);
					
		SendTrace(INFO_LEVEL, "Y Axis Acc = %fg\t", AccTiltData->AccY);
					
		SendTrace(INFO_LEVEL, "Z Axis Acc = %fg\r\n", AccTiltData->AccZ);
				
		SendTrace(INFO_LEVEL, "X Axis Tilt = %f degrees\t", AccTiltData->TiltX);
				
		SendTrace(INFO_LEVEL, "Y Axis Tilt = %f degrees\t", AccTiltData->TiltY);
				
		SendTrace(INFO_LEVEL, "Z Axis Tilt = %f degrees\r\n\r\n", AccTiltData->TiltZ);	
	
		Status = TRUE;
	}
	
Exit:

	return Status;
}
