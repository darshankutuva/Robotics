/*
 * ADXL345.c
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

#include <avr/io.h>

#include <stdlib.h>

#include <math.h>

#include "ADXL345.h"

#include "ADXL345_Internal.h"

#include "TWI.h"

#include "Trace.h"

bool gIsADXL345Initialized = FALSE;

bool gIsADXL345InStandby = TRUE;

TWI_ADDRESS gADXL345DeviceAddress;

bool InitADXL345DevAddress(bool IsAltAddressGnd)
{
	bool Status = FALSE;
	
    gADXL345DeviceAddress.Address = GET_DEVICE_ADDRESS(IsAltAddressGnd);
	
	SendTrace(INFO_LEVEL, "Device Address = 0x%x\r\n", gADXL345DeviceAddress.Address);
	
	//
	// Check Accelerometer Device ID
	//
	
	Status = CheckADXL345DeviceId();
	
	if (!Status)
	{
		goto Exit;
	}
	
	gIsADXL345Initialized = TRUE;
	
Exit:
	
    return Status;
}

bool EnableDisableADXL345StandbyMode(bool Enable)
{
	bool Status = FALSE;
	
	POWER_CTL PowerCtl;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	//
	// Read Power Ctl Register
	//
	
	Status = ReadADXL345Register(ADR_POWER_CTL, &PowerCtl, sizeof(POWER_CTL));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Power CTL register. \r\n");
		
		goto Exit;
	}
	
	//
	// Now place the accelerometer in standby mode
	//
	
	PowerCtl.Measure = (Enable ? 0 : 1);
	
	Status = SetADXL345Register(ADR_POWER_CTL, &PowerCtl, sizeof(POWER_CTL));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to place Accelerometer in standby mode. \r\n");
		
		goto Exit;
	}
	
	gIsADXL345InStandby = Enable;
	
Exit:

	return Status;
}

bool ProgramADXL345PowerCtl(POWER_MODE PowerMode, WAKEUP_FREQ WakeupFreq)
{
	bool Status = FALSE;
	
	BW_RATE BwRate;
	
	POWER_CTL PowerCtl;
	
	if (!gIsADXL345Initialized || !gIsADXL345InStandby)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized or it is not in standby mode. \r\n");
		
		goto Exit;
	}
	
	if (PowerMode == FULL_POWER && WakeupFreq != NO_WAKEUP)
	{
		SendTrace(ERROR_LEVEL, "Invalid Wakeup Frequency Selected.\r\n");
		
		goto Exit;
	}
	
	//
	// Read BW Rate Register
	//
	
	Status = ReadADXL345Register(ADR_BW_RATE, &BwRate, sizeof(BW_RATE));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read BW RATE register. \r\n");
		
		goto Exit;
	}
	
	//
	// Program Power Ctl
	//
	
	PowerCtl.AsUINT8 = 0;
	
	switch (PowerMode)
	{
		case LOW_POWER:
		{
			if (BwRate.Rate < TWELVE_POINT_FIVE_HZ || BwRate.Rate > FOUR_HUNDRED_HZ)
			{
				SendTrace(ERROR_LEVEL, "Invalid Data Rate for Low Power Mode. \r\n");
				
				Status = FALSE;
				
				goto Exit;
			}
			
			BwRate.LowPwr = 1;
			
			PowerCtl.AutoSleep = 1;
			
			PowerCtl.Link = 1;
			
			PowerCtl.Measure = 1;
			
			PowerCtl.Wakeup = WakeupFreq;
			
			break;
		}
		case AUTO_SLEEP:
		{
			BwRate.LowPwr = 0;
			
			PowerCtl.AutoSleep = 1;
			
			PowerCtl.Link = 1;
			
			PowerCtl.Measure = 1;
			
			PowerCtl.Wakeup = WakeupFreq;
			
			break;
		}
		case FULL_POWER:
		{
			BwRate.LowPwr = 0;
			
			PowerCtl.Measure = 1;
			
			break;
		}
	};
	
	Status = SetADXL345Register(ADR_BW_RATE, &BwRate, sizeof(BW_RATE));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program BW RATE Register. \r\n");
		
		goto Exit;
	}
	
	Status = SetADXL345Register(ADR_POWER_CTL, &PowerCtl, sizeof(POWER_CTL));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program Power CTL Register. \r\n");
	}
	
Exit:

	return Status;
}

bool ProgramADXL345Interrupts(PINT_ENABLE_MAP_SOURCE IntMap, PINT_ENABLE_MAP_SOURCE IntEnable)
{
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	//
	// Program Interrupts
	//
	
	Status = SetADXL345Register(ADR_INT_MAP, IntMap, sizeof(INT_ENABLE_MAP_SOURCE));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program INT MAP Register. \r\n");
		
		goto Exit;
	}
	
	Status = SetADXL345Register(ADR_INT_ENABLE, IntEnable, sizeof(INT_ENABLE_MAP_SOURCE));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program INT ENABLE Register. \r\n");
	}
	
Exit:

	return Status;
}

bool ProgramADXL345FifoControl(PFIFO_CTL FifoCtl)
{
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	//
	// Program FIFO Control Register
	//
	
	Status = SetADXL345Register(ADR_FIFO_CTL, FifoCtl, sizeof(FIFO_CTL));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program FIFO CTL Register. \r\n");
	}
	
Exit:
	
	return Status;
}

bool ProgramADXL345DataFormat(DATA_RANGE DataRange, bool IntInvert, PDATA_OFFSET DataOffset, DATA_RATE DataRate)
{
	
	bool Status = FALSE;
	
	DATA_FORMAT DataFormat;
	
	BW_RATE BwRate;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	//
	// Set Data Format
	//
	
	DataFormat.FullRes = 1;
	
	DataFormat.IntInvert = IntInvert;
	
	DataFormat.Justify = 0;
	
	DataFormat.Range = DataRange;
	
	DataFormat.Rsvd = 0;
	
	DataFormat.SelfTest = 0;
	
	DataFormat.SPI = 0;
	
	Status = SetADXL345Register(ADR_DATA_FORMAT, &DataFormat, sizeof(DATA_FORMAT));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program DATA FORMAT Register. \r\n");
		
		goto Exit;
	}
	
	//
	// Read Data Rate Register
	//
	
	Status = ReadADXL345Register(ADR_BW_RATE, &BwRate, sizeof(BW_RATE));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read BW RATE register. \r\n");
		
		goto Exit;
	}
	
	BwRate.Rate = DataRate;
	
	//
	// Program new data rate
	//
	
	Status = SetADXL345Register(ADR_BW_RATE, &BwRate, sizeof(BW_RATE));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to set BW RATE register. \r\n");
		
		goto Exit;
	}
	
	//
	// Set Data Offset
	//
	
	Status = SetADXL345Register(ADR_OFSX, &DataOffset->OffsetX, sizeof(signed char));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program DATA OFFSET X Register. \r\n");
		
		goto Exit;
	}
	
	Status = SetADXL345Register(ADR_OFSY, &DataOffset->OffsetY, sizeof(signed char));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program DATA OFFSET Y Register. \r\n");
		
		goto Exit;
	}
	
	Status = SetADXL345Register(ADR_OFSZ, &DataOffset->OffsetZ, sizeof(signed char));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program DATA OFFSET Z Register. \r\n");
	}
	
Exit:

	return Status;
}

bool ProgramADXL345ActivityInactivityDetection(PACT_INACT_CTL ActInactCtl, unsigned char ActThreshold, unsigned char InactThreshold, unsigned char TimeInact)
{
	
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	//
	// Program Act Inact Ctl Register
	//
	
	Status = SetADXL345Register(ADR_INACT_CTL, ActInactCtl, sizeof(ACT_INACT_CTL));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program ACT INACT CTL Register. \r\n");
		
		goto Exit;
	}
	
	//
	// Program Act Threshold Register
	//
	
	Status = SetADXL345Register(ADR_THRESH_ACT, &ActThreshold, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace (ERROR_LEVEL, "Failed to program THRESH ACT Register. \r\n");
		
		goto Exit;
	}
	
	//
	// Program Inact Threshould Register
	//
	
	Status = SetADXL345Register(ADR_THRESH_INACT, &InactThreshold, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program THRESH INACT Register. \r\n");
		
		goto Exit;
	}
	
	//
	// Program Time Inact Register
	//
	
	Status = SetADXL345Register(ADR_TIME_INACT, &TimeInact, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program TIME INACT Register. \r\n");
	}
	
Exit:
	
	return Status;
}

bool ProgramADXL345FreeFallDetection(unsigned char FreeFallThreshold, unsigned char FreeFallTime)
{
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	Status = ReadADXL345Register(ADR_THRESH_FF, &FreeFallThreshold, sizeof(unsigned char));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program FREE FALL THRESHOLD register. \r\n");
		
		goto Exit;
	}
	
	Status = ReadADXL345Register(ADR_TIME_FF, &FreeFallTime, sizeof(unsigned char));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program Free Fall Threshold Time register. \r\n");
	}
	
Exit:

	return Status;
}

bool ProgramADXL345TapDetection(PTAP_AXES TapAxes, unsigned char Duration, unsigned char Latency, unsigned char Window)
{
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	Status = SetADXL345Register(ADR_TAP_AXES, &TapAxes, sizeof(TAP_AXES));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to set TAP AXES register. \r\n");
		
		goto Exit;
	}

	Status = SetADXL345Register(ADR_DUR, &Duration, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to set Duration register. \r\n");
		
		goto Exit;
	}
	
	Status = SetADXL345Register(ADR_Latent, &Latency, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to set Latency register. \r\n");
		
		goto Exit;
	}
	
	Status = SetADXL345Register(ADR_Window, &Window, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to set Window register. \r\n");
	}
	
Exit:
	
	return Status;
}

bool ReadADXL345InterruptSource(PINT_ENABLE_MAP_SOURCE IntSource)
{
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	Status = ReadADXL345Register(ADR_INT_SOURCE, IntSource, sizeof(INT_ENABLE_MAP_SOURCE));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read INT SOURCE register. \r\n");
	}
	
Exit:
	
	return Status;
}

bool ReadADXL345FifoStatus(PFIFO_STATUS FifoStatus)
{
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	Status = ReadADXL345Register(ADR_FIFO_STATUS, FifoStatus, sizeof(FIFO_STATUS));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read FIFO STATUS register. \r\n");
	} 
	
Exit:
	
	return Status;
}

bool ReadADXL345ActTapStatus(PACT_TAP_STATUS ActTapStatus)
{
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	Status = ReadADXL345Register(ADR_ACT_TAP_STATUS, ActTapStatus, sizeof(ACT_TAP_STATUS));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read ACT TAP Status register. \r\n");
	}
	
Exit:

	return Status;
}

bool ReadADXL345RawData(PRAW_DATA_XYZ RawData)
{
	bool Status = FALSE;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	Status = ReadADXL345Register(ADR_DATAX0, RawData, sizeof(RAW_DATA_XYZ));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Data Register. \r\n");
	}
	
Exit:

	return Status;
}

bool ReadADXL345AccAndTiltData(ANGLE_UNIT AngleUnit, PACC_TILT_DATA_XYZ AccTiltData)
{
	bool Status = FALSE;
	
	double DataXSqrd;
	
	double DataYSqrd;
	
	double DataZSqrd;
	
	double RadToDegree;
	
	RAW_DATA_XYZ RawData;
	
	if (!gIsADXL345Initialized)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer not initialized. \r\n");
		
		goto Exit;
	}
	
	Status = ReadADXL345Register(ADR_DATAX0, &RawData, sizeof(RAW_DATA_XYZ));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Data Register. \r\n");
		
		goto Exit;
	}
	
	//
	// Calculate G Data in X, Y and Z axis
	//
	
	AccTiltData->AccX = RawData.DataX / LsbPerG;
	
	AccTiltData->AccY = RawData.DataY / LsbPerG;
	
	AccTiltData->AccZ = RawData.DataZ / LsbPerG;
	
	//
	// Calculate Tilt in X, Y and Z axis
	//
	
	DataXSqrd = pow(AccTiltData->AccX, 2.0);
	
	DataYSqrd = pow(AccTiltData->AccY, 2.0);
	
	DataZSqrd = pow(AccTiltData->AccZ, 2.0);
	
	AccTiltData->TiltX = atan2 (AccTiltData->AccX, sqrt(DataYSqrd + DataZSqrd));
	
	AccTiltData->TiltY = atan2 (AccTiltData->AccY, sqrt(DataXSqrd + DataZSqrd));
	
	AccTiltData->TiltZ = atan2 (sqrt(DataXSqrd + DataYSqrd), AccTiltData->AccZ);
	
	if (AngleUnit == DEGREES)
	{
		RadToDegree = 180.0 / M_PI;
		
		AccTiltData->TiltX = AccTiltData->TiltX * RadToDegree;
	
		AccTiltData->TiltY = AccTiltData->TiltY * RadToDegree;
	
		AccTiltData->TiltZ = AccTiltData->TiltZ * RadToDegree;
	}
	
Exit:

	return Status;	
}

bool FlushADXL345Fifo()
{
	bool Status = TRUE;
	
	FIFO_STATUS FifoStatus;
	
	unsigned char index;
	
	RAW_DATA_XYZ Data;
	
	if (!ReadADXL345FifoStatus(&FifoStatus))
	{
		Status = FALSE;
		
		goto Exit;
	}
	
	for (index=0; index < FifoStatus.Entries; index++)
	{
		if(!ReadADXL345RawData(&Data))
		{
			SendTrace(ERROR_LEVEL, "Reading data failed while attempting to flush FIFO. \r\n");
		}
	}
	
Exit:

	return Status;
}

//
// Private Functions
//

bool CheckADXL345DeviceId()
{
	bool Status = FALSE;
	
	unsigned char DeviceId;
	
	Status = ReadADXL345Register(ADR_DEVID, &DeviceId, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Accelerometer Device Id. \r\n");
		
		goto Exit;
	}
	
	if (DeviceId != DEVID)
	{
		SendTrace(ERROR_LEVEL, "Accelerometer Device Id does not match 0x%x. \r\n", DeviceId);
		
		Status = FALSE;
	}
	
Exit:
	
	return Status;
}

bool ReadADXL345Register(unsigned char RegisterAddress, PVOID RegisterValue, unsigned long Length)
{
	
	bool Status = FALSE;
		
	Status = TwiTransmit(&RegisterAddress, sizeof (unsigned char), &gADXL345DeviceAddress, FALSE);
		
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to transmit Register Address. \r\n");
			
		goto Exit;
	}
		
	Status = TwiReceive(RegisterValue, Length, &gADXL345DeviceAddress, TRUE);
		
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to retrieve Register Value. \r\n");		
	}
		
Exit:

	return Status;
}

bool SetADXL345Register(unsigned char RegisterAddress, PVOID RegisterValue, unsigned long Length)
{
	bool Status = FALSE;
	
	PVOID WriteValue;
	
	WriteValue = malloc(Length + 1);
	
	if (WriteValue == NULL)
	{
		goto Exit;
	}
	
	*(unsigned char *)(WriteValue) = RegisterAddress;
	
	memcpy(WriteValue + 1, RegisterValue, Length);
	
	Status = TwiTransmit(WriteValue, Length + 1, &gADXL345DeviceAddress, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to transmit Register Value. \r\n");
	}
	
	free(WriteValue);
	
	SendTrace(INFO_LEVEL, "Written Value 0x%x\t", *(unsigned char *)RegisterValue);
	
	ReadADXL345Register(RegisterAddress, RegisterValue, Length);
	
	SendTrace(INFO_LEVEL, "Read Value 0x%x\r\n", *(unsigned char *)RegisterValue);
	
Exit:

	return Status;
}

