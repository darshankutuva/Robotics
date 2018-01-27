/*
 * AccelerometerTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 


#include "Common.h"

#include "ADXL345.h"

#include "TWI.h"

#include "Trace.h"

bool IsAccelerometerInterruptFired;

int main(void)
{
	
	INT_ENABLE_MAP_SOURCE IntEnable;
	
	INT_ENABLE_MAP_SOURCE IntMap;
	
	DATA_OFFSET DataOffset;
	
	FIFO_CTL FifoCtl;
	
	ACT_INACT_CTL ActInactCtl;
	
	INT_ENABLE_MAP_SOURCE IntSource;
	
	FIFO_STATUS FifoStatus;
	
	//RAW_DATA_XYZ Data;
	
	ACC_TILT_DATA_XYZ AccTiltData;
	
	unsigned char index;
	
	//
	// Wait for serial connection to be established
	//
	
	_delay_ms(15000);
	
	//
	// Initialize Tracing
	//
	
	InitTrace(F_CPU, ERROR_LEVEL | INFO_LEVEL);
	
	//
	// Initialize TWI
	//

	if(!InitTWI(TWI_MASTER, NULL, NULL))
	{
		goto Exit;
	}
	
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
	
	FifoCtl.FIFOMode = FIFO;
	
	FifoCtl.Samples = 20;
	
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
	
	if(!ProgramADXL345DataFormat(FOUR_G, TRUE, &DataOffset, HUNDRED_HZ))
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
	
	IsAccelerometerInterruptFired = FALSE;
	
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
	
	while(1)
	{
		if (!ReadADXL345InterruptSource(&IntSource))
		{
			continue;
		}
		
		SendTrace(INFO_LEVEL, "Interrupt Source = 0x%x\r\n", IntSource.AsUINT8);
		
		if (IsAccelerometerInterruptFired || IntSource.DataReady)
		{
			IsAccelerometerInterruptFired = FALSE;
			
			if (!ReadADXL345FifoStatus(&FifoStatus))
			{
				continue;
			}
				
			SendTrace(INFO_LEVEL, "Fifo Entries = %d\r\n", FifoStatus.Entries);
				
			SendTrace(INFO_LEVEL, "Fifo Triggered = %d\r\n", FifoStatus.FIFOTrig); 
				
			for (index=0; index < FifoStatus.Entries; index++)
			{
				/*if (!ReadRawData(&Data))
				{
					continue;
				}
				
				SendTrace(INFO_LEVEL, "X Axis Data = %dg\t", Data.DataX);
				
				SendTrace(INFO_LEVEL, "Y Axis Data = %dg\t", Data.DataY);
				
				SendTrace(INFO_LEVEL, "Z Axis Data = %dg\r\n", Data.DataZ);*/
				
				if(!ReadADXL345AccAndTiltData(DEGREES, &AccTiltData))
				{
					continue;
				}
					
				SendTrace(INFO_LEVEL, "X Axis Acc = %fg\t", AccTiltData.AccX);
					
				SendTrace(INFO_LEVEL, "Y Axis Acc = %fg\t", AccTiltData.AccY);
					
				SendTrace(INFO_LEVEL, "Z Axis Acc = %fg\r\n", AccTiltData.AccZ);
				
				SendTrace(INFO_LEVEL, "X Axis Tilt = %f degrees\t", AccTiltData.TiltX);
				
				SendTrace(INFO_LEVEL, "Y Axis Tilt = %f degrees\t", AccTiltData.TiltY);
				
				SendTrace(INFO_LEVEL, "Z Axis Tilt = %f degrees\r\n\r\n", AccTiltData.TiltZ);
			}	
			
		}
		/*else
		{
			SendTrace(INFO_LEVEL, "PCIFR Register = 0x%x\r\n", PCIFR);
			
			if (!ReadADXL345FifoStatus(&FifoStatus))
			{
				continue;
			}
			
			SendTrace(INFO_LEVEL, "Fifo Entries = %d\r\n", FifoStatus.Entries);
			
			if (!ReadADXL345InterruptSource(&IntSource))
			{
				continue;
			}
			
			SendTrace(INFO_LEVEL, "Interrupt Source = 0x%x\r\n", IntSource.AsUINT8);
		}*/
	}
	
Exit:

	return 1;
}

ISR(PCINT2_vect)
{
	SendTrace(INFO_LEVEL, "Interrupt Fired\r\n");
	
	IsAccelerometerInterruptFired = TRUE;
}