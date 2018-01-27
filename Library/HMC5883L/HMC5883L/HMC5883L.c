/*
 * HMC5883L.c
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

#include <math.h>

#include "HMC5883L.h"

#include "HMC5883L_Internal.h"

#include "TWI.h"

#include "Trace.h"

bool gIsInitialized = FALSE;

TWI_ADDRESS gHMC5883LDeviceAddress;

OP_MODE gHMC5883LOpMode;

//
// Public Functions
//

bool InitHMC5883LMagnetometer(OP_MODE OpMode, 
							  MEASUREMENT_MODE MeasurementMode,
							  DATA_OUTPUT_RATE DataOutputRate,
							  SAMPLES_AVERAGED_PER_MEASUREMENT SamplesAveragedPerMeasurement,
							  GAIN_CONFIGURATION GainConfiguration)
{
	bool Status = FALSE;
	
	CONFIG_REG_A ConfigRegA;
	
	CONFIG_REG_B ConfigRegB;
	
	MODE_REG ModeReg;
	
	gHMC5883LDeviceAddress.Address = DEVICE_ADDRESS;
	
	if (gIsInitialized)
	{
		//
		// Program Mode Register
		//
		
		ModeReg.AsUINT8 = 0;
		
		ModeReg.Mode = IDLE_MODE;
		
		Status = SetHMC5883LRegister(ADR_MODE_REG, &ModeReg, sizeof(MODE_REG));
		
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to program MODE REG to stop measurement. \r\n");
			
			goto Exit;
		}
		
		//
		// Stop measurement.
		//
		
		ConfigRegA.AsUINT8 = 0;
		
		ConfigRegA.StartStopMeasurement = 1;
		
		Status = SetHMC5883LRegister(ADR_CONFIG_REG_A, &ConfigRegA, sizeof(CONFIG_REG_A));
		
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to program Config Reg A to stop measurement. \r\n");
			
			goto Exit;
		}
	}
	else
	{
		if (!CheckHMC5883LDeviceId())
		{
			goto Exit;
		}
		else
		{
			SendTrace(INFO_LEVEL, "Device ID Check Passed.\r\n");
		}
	}
	
	//
	// Program Mode Register
	//
	
	ModeReg.AsUINT8 = 0;
	
	ModeReg.Mode = OpMode;
	
	Status = SetHMC5883LRegister(ADR_MODE_REG, &ModeReg, sizeof(MODE_REG));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program MODE REG. \r\n");
		
		goto Exit;
	}
	
	gHMC5883LOpMode = OpMode;
	
	//
	// Program Configuration Register B
	//
	
	ConfigRegB.AsUINT8 = 0;
	
	ConfigRegB.GainConfiguration = GainConfiguration;
	
	Status = SetHMC5883LRegister(ADR_CONFIG_REG_B, &ConfigRegB, sizeof(CONFIG_REG_B));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program CONFIG REG B. \r\n");
		
		goto Exit;
	}
	
	//
	// Program Configuration Register A
	//
	
	ConfigRegA.DataOutputRate = DataOutputRate;
	
	ConfigRegA.MeasurementMode = MeasurementMode;
	
	ConfigRegA.SamplesAveragedPerMeasurement = SamplesAveragedPerMeasurement;
	
	Status = SetHMC5883LRegister(ADR_CONFIG_REG_A, &ConfigRegA, sizeof(CONFIG_REG_A));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program CONFIG REG A. \r\n");
		
		goto Exit;
	}
	
	PrintHMC5883LConfigRegisters();
	
	gIsInitialized = TRUE;
	
Exit:

	return Status;
}

bool ReadHMC5883LContinuousMeasurementData(PMAGNETOMETER_DATA pData)
{
	bool Status = FALSE;
	
	if (!gIsInitialized)
	{
		SendTrace(ERROR_LEVEL, "Magnetometer not Initialized. \r\n");
		
		goto Exit;
	}
	
	if (gHMC5883LOpMode != CONTINUOUS_MEASUREMENT_MODE)	
	{
		SendTrace(ERROR_LEVEL, "Magnetometer not in CONTINUOUS MEASUREMENT Mode.\r\n");
		
		goto Exit;	
	}	
	
	if(IsHMC5883LDataReady())
	{	
		//
		// Initiate Data Read
		//
		
		Status = ReadHMC5883LData(pData);
		
		if (!Status)
		{
			SendTrace(INFO_LEVEL, "Reading HMC5883L Data Failed\r\n");
			
			goto Exit;
		}
		
		pData->GaussX = pData->RawDataX * HMC5883L_SCALE_FACTOR;
		
		pData->GaussY = pData->RawDataY * HMC5883L_SCALE_FACTOR;
		
		pData->GaussZ = pData->RawDataZ * HMC5883L_SCALE_FACTOR;
	}
	else
	{
		SendTrace(INFO_LEVEL, "Data not ready yet. \r\n");
		
		Status = FALSE;
	}
	
Exit:

	return Status;
}

bool ReadHMC5883LSingleMeasurementData(PMAGNETOMETER_DATA pData)
{
	bool Status = FALSE;
	
	MODE_REG ModeReg;
	
	if (!gIsInitialized)
	{
		SendTrace(ERROR_LEVEL, "Magnetometer not Initialized. \r\n");
		
		goto Exit;
	}

	if (gHMC5883LOpMode != SINGLE_MEASUREMENT_MODE)
	{
		SendTrace(ERROR_LEVEL, "Magnetometer not in SINGLE MEASUREMENT Mode.\r\n");
	
		goto Exit;
	}
	
	//
	// Initiate Single Measurement.
	//
	
	ModeReg.AsUINT8 = 0;
	
	ModeReg.Mode = SINGLE_MEASUREMENT_MODE;
	
	Status = SetHMC5883LRegister(ADR_MODE_REG, &ModeReg, sizeof(MODE_REG));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to initiate Single Measurement. \r\n");
		
		goto Exit;
	}
	
	//
	// Loop Until Data is ready.
	//

	while (!IsHMC5883LDataReady());
	
	//
	// Initiate Data Read
	//
	
	Status = ReadHMC5883LData(pData);
	
	if (!Status)
	{
		SendTrace(INFO_LEVEL, "Reading HMC5883L Data Failed\r\n");
		
		goto Exit;
	}
	
	pData->GaussX = pData->RawDataX * HMC5883L_SCALE_FACTOR;
	
	pData->GaussY = pData->RawDataY * HMC5883L_SCALE_FACTOR;
	
	pData->GaussZ = pData->RawDataZ * HMC5883L_SCALE_FACTOR;
	
Exit:

	return Status;
}

bool CalculateCompassHeading(PMAGNETOMETER_DATA pMagData, 
							 double Theta, 
							 double Phi, 
							 ANGLE_UNIT AngleUnit, 
							 double *pCompassHeading)
{
	bool Status = TRUE;
	
	double MagX;
	
	double MagY;
	
	double CosTheta;
	
	double SinTheta;
	
	double CosPhi;
	
	double SinPhi;
	
	if (pMagData == NULL || pCompassHeading == NULL)
	{
		Status = FALSE;
		
		goto Exit;
	}
	
	//
	// We need to rotate the object in the opposite X and Y direction to align the object in X and Y axis.
	//
	
	Theta = Theta * -1;
	
	//Phi = Phi * -1;
	
	CosTheta = cos(Theta);
	
	SinTheta = sin(Theta);
	
	CosPhi = cos(Phi);
	
	SinPhi = sin(Phi);
	
	MagX = (pMagData->GaussX * CosTheta) + (pMagData->GaussY * SinTheta * SinPhi) + (pMagData->GaussZ * SinTheta * CosPhi);
	
	MagY = (pMagData->GaussY * CosPhi) - (pMagData->GaussZ * SinPhi);
	
	*pCompassHeading = atan2(MagX, MagY);
	
	//*pCompassHeading = atan2(pMagData->GaussX, pMagData->GaussY); // - ((M_PI / 180) * 24);
	
	*pCompassHeading = *pCompassHeading + M_PI;
	
	if (*pCompassHeading < 0)
	{
		*pCompassHeading = *pCompassHeading + (2 * M_PI);
	}
	else if (*pCompassHeading > (2 * M_PI))
	{
		*pCompassHeading = *pCompassHeading - (2 * M_PI);
	}
	
	*pCompassHeading = (2 * M_PI) - *pCompassHeading;
	
	if (AngleUnit == DEGREES)
	{
		*pCompassHeading = *pCompassHeading * (180.0 / M_PI);
	}
	
Exit:

	return Status;
}

bool PrintHMC5883LConfigRegisters()
{
	bool Status;
	
	unsigned char Value;
	
	Status = ReadHMC5883LRegister(ADR_CONFIG_REG_A, &Value, sizeof(unsigned char));
	
	if(!Status)
	{
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "CONFIG REG A = 0x%x\r\n", Value);
	
	Status = ReadHMC5883LRegister(ADR_CONFIG_REG_B, &Value, sizeof(unsigned char));
	
	if(!Status)
	{
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "CONFIG REG B = 0x%x\r\n", Value);
	
	Status = ReadHMC5883LRegister(ADR_MODE_REG, &Value, sizeof(unsigned char));
	
	if(!Status)
	{
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "MODE REG = 0x%x\r\n", Value);
	
Exit:

	return Status;
}

//
// Private Functions
//

bool IsHMC5883LDataReady()
{
	bool Status;
	
	STATUS_REG StatusReg;
	
	Status = ReadHMC5883LRegister(ADR_STATUS_REG, &StatusReg, sizeof(STATUS_REG));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Status Reg. \r\n");
		
		goto Exit;
	}
	
	if (StatusReg.Ready)
	{
		//
		// Lock the Data Registers
		//
		
		StatusReg.Lock = 1;
		
		Status = SetHMC5883LRegister(ADR_STATUS_REG, &StatusReg, sizeof(STATUS_REG));
		
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to lock Status Reg. \r\n");
			
			goto Exit;
		}
	}
	else
	{
		Status = FALSE;
	}
	
Exit:
	
	return Status;
}

bool ReadHMC5883LData(PMAGNETOMETER_DATA pData)
{
	
	bool Status;
	
	char Hmc5333lData[6];
	
	//
	// Read Data
	//
	
	Status = ReadHMC5883LRegister(ADR_DATA_OUT_X_MSB_REG, &Hmc5333lData[0], 6);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Data register. \r\n");
		
		goto Exit;
	}
	
	pData->RawDataX = (Hmc5333lData[0] << 8) | Hmc5333lData[1];
	
	pData->RawDataZ = (Hmc5333lData[2] << 8) | Hmc5333lData[3];
	
	pData->RawDataY = (Hmc5333lData[4] << 8) | Hmc5333lData[5]; 
	
Exit:

	return Status;
}

bool ReadHMC5883LRegister(unsigned char RegisterAddress, PVOID pRegisterValue, unsigned long Length)
{
	
	bool Status;
	
	Status = TwiTransmit(&RegisterAddress, sizeof (unsigned char), &gHMC5883LDeviceAddress, FALSE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to transmit Register Address. \r\n");
		
		goto Exit;
	}
	
	Status = TwiReceive(pRegisterValue, Length, &gHMC5883LDeviceAddress, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to retrieve Register Value. \r\n");
	}
	
Exit:

	return Status;
}

bool SetHMC5883LRegister(unsigned char RegisterAddress, PVOID pRegisterValue, unsigned long Length)
{
	bool Status;
	
	unsigned char TwiData[Length + 1];
	
	unsigned long index;
	
	
	TwiData[0] = RegisterAddress;
	
	for (index = 0; index < Length; index ++)
	{
		TwiData[index + 1] = *((unsigned char *)pRegisterValue + index);
	}
	
	Status = TwiTransmit(&TwiData[0], Length + 1, &gHMC5883LDeviceAddress, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to transmit Register Address. \r\n");
	}

	return Status;
}

bool CheckHMC5883LDeviceId()
{
	bool Status = FALSE;
	
	unsigned char DeviceId = 0;
	
	Status = ReadHMC5883LRegister(ADR_ID_REG_A, &DeviceId, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Magnetometer Device Id A. \r\n");
		
		goto Exit;
	}
	
	if (DeviceId != ID_A)
	{
		SendTrace(ERROR_LEVEL, "Magnetometer Device Id A does not match 0x%x. \r\n", DeviceId);
		
		Status = FALSE;
	}
	
	Status = ReadHMC5883LRegister(ADR_ID_REG_B, &DeviceId, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Magnetometer Device Id B. \r\n");
		
		goto Exit;
	}
	
	if (DeviceId != ID_B)
	{
		SendTrace(ERROR_LEVEL, "Magnetometer Device Id B does not match 0x%x. \r\n", DeviceId);
		
		Status = FALSE;
	}
	
	Status = ReadHMC5883LRegister(ADR_ID_REG_C, &DeviceId, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Magnetometer Device Id C. \r\n");
		
		goto Exit;
	}
	
	if (DeviceId != ID_C)
	{
		SendTrace(ERROR_LEVEL, "Magnetometer Device Id C does not match 0x%x. \r\n", DeviceId);
		
		Status = FALSE;
	}
	
Exit:
	
	return Status;
}