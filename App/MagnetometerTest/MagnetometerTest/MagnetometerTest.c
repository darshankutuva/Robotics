/*
 * MagnetometerTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 


#include "MagnetometerTest.h"

#include "ADXL345.h"

#include "HMC5883L.h"

bool IsAccelerometerInterruptFired;

int main(void)
{
	INT_ENABLE_MAP_SOURCE IntSource;
	
	FIFO_STATUS FifoStatus;
	
	//RAW_DATA_XYZ Data;
	
	ACC_TILT_DATA_XYZ AccTiltData;
	
	MAGNETOMETER_DATA MagData;
	
	double CompassHeading;
	
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

	if (!InitTWI(TWI_MASTER, NULL, NULL))
	{
		goto Exit;
	}
	
	//
	// Initialize Accelerometer
	//
	
	if (!InitializeAccelerometer())
	{
		goto Exit;
	}
	
	if (!InitHMC5883LMagnetometer(SINGLE_MEASUREMENT_MODE, NORMAL, FIFTEEN_HZ, EIGHT_SAMPLES, THOUSAND_AND_NINTY))
	{
		goto Exit;
	}
	
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
					
				if(!ReadADXL345AccAndTiltData(RADS, &AccTiltData))
				{
					continue;
				}
				
				if (!ReadHMC5883LSingleMeasurementData(&MagData))
				{
					continue;	
				}
				
				CalculateCompassHeading(&MagData, AccTiltData.TiltX, AccTiltData.TiltY, DEGREES, &CompassHeading);
					
				SendTrace(INFO_LEVEL, "X Axis Acc = %fg\t", AccTiltData.AccX);
					
				SendTrace(INFO_LEVEL, "Y Axis Acc = %fg\t", AccTiltData.AccY);
					
				SendTrace(INFO_LEVEL, "Z Axis Acc = %fg\r\n", AccTiltData.AccZ);
				
				SendTrace(INFO_LEVEL, "X Axis Tilt = %f degrees\t", AccTiltData.TiltX);
				
				SendTrace(INFO_LEVEL, "Y Axis Tilt = %f degrees\t", AccTiltData.TiltY);
				
				SendTrace(INFO_LEVEL, "Z Axis Tilt = %f degrees\r\n", AccTiltData.TiltZ);
				
				SendTrace(INFO_LEVEL, "X Axis Raw = %dG\t", MagData.RawDataX);
				
				SendTrace(INFO_LEVEL, "Y Axis Raw = %dG\t", MagData.RawDataY);
				
				SendTrace(INFO_LEVEL, "Z Axis Raw = %dG\r\n", MagData.RawDataZ);
				
				SendTrace(INFO_LEVEL, "X Axis Mag = %fG\t", MagData.GaussX);
				
				SendTrace(INFO_LEVEL, "Y Axis Mag = %fG\t", MagData.GaussY);
				
				SendTrace(INFO_LEVEL, "Z Axis Mag = %fG\r\n", MagData.GaussZ);
				
				SendTrace(INFO_LEVEL, "Compass Heading = %f degrees\r\n\r\n", CompassHeading);
			}
		}
    }
	
Exit:

	return 1;
	
}

bool InitializeAccelerometer()
{
	bool Status = FALSE;
	
	INT_ENABLE_MAP_SOURCE IntEnable;
		
	INT_ENABLE_MAP_SOURCE IntMap;
		
	DATA_OFFSET DataOffset;
		
	FIFO_CTL FifoCtl;
		
	ACT_INACT_CTL ActInactCtl;
	
		
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

ISR(PCINT2_vect)
{
	SendTrace(INFO_LEVEL, "Interrupt Fired\r\n");
	
	IsAccelerometerInterruptFired = TRUE;
}

