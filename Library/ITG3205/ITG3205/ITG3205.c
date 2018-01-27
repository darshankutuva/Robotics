/*
 * ITG3205.c
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

#include "ITG3205.h"

#include "ITG3205_Internal.h"

#include "TWI.h"

#include "Trace.h"

bool gIsItg3205Initialized = FALSE;

TWI_ADDRESS gItg3205DeviceAddress;


bool InitItg3205(ITG3205_DLPF_CFG DlpfCfg, 
				 unsigned char SmplrtDiv, 
				 PITG3205_INTERRUPT_CONFIG pInterruptConfig, 
				 ITG3205_CLK_SEL ClkSel,
				 bool IsPin9Gnd)
{
	bool Status = FALSE;
	
	
	gItg3205DeviceAddress.Address = GET_ITG3205_DEVICE_ADDRESS(IsPin9Gnd);
	
	SendTrace(INFO_LEVEL, "Device Address = 0x%x\r\n", gItg3205DeviceAddress.Address);
	
	if (!CheckItg3205Id())
	{
		goto Exit;
	}
	
	if(!ConfigureItg3205DigitalLowPassFilter(DlpfCfg))
	{
		goto Exit;
	}
	
	if(!ConfigureItg3205SampleRateDivider(SmplrtDiv))
	{
		goto Exit;
	}
	
	if(!ConfigureItg3205Interrupt(pInterruptConfig))	
	{
		goto Exit;
	}
	
	if(!ConfigureItg3205Clock(ClkSel))
	{
		goto Exit;
	}
	
	PrintItg3205ConfigRegisters();
	
	Status = TRUE;
	
	gIsItg3205Initialized = TRUE;
	
Exit:

	return Status;
}

bool ConfigureItg3205Interrupt(PITG3205_INTERRUPT_CONFIG pInterruptConfig)
{
	bool Status = FALSE;
	
	Status = SetItg3205Register(ADR_INTERRUPT_CONFIG, pInterruptConfig, sizeof(ITG3205_INTERRUPT_CONFIG));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program Interrupt Config Register. \r\n");
	}

	return Status;
}

bool ResetItg3205()
{
	bool Status = FALSE;
	
	ITG3205_PWR_MGMT PwrMgmt;
	
	if (!gIsItg3205Initialized)
	{
		goto Exit;
	}
	
	PwrMgmt.AsUINT8 = 0;
	
	PwrMgmt.HReset = 1;
	
	Status = SetItg3205Register(ADR_PWR_MGMT, &PwrMgmt, sizeof(ITG3205_PWR_MGMT));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program Power Register. \r\n");
		
		goto Exit;
	}
	
	while(PwrMgmt.HReset)
	{
		Status = ReadItg3205Register(ADR_PWR_MGMT, &PwrMgmt, sizeof(ITG3205_PWR_MGMT));
		
		if(!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to read Power Register. \r\n");
			
			goto Exit;
		}
	}
	
Exit:

	return Status;
}

bool EnableDisableItg3205Sleep(bool Enable)
{
	bool Status = FALSE;
	
	ITG3205_PWR_MGMT PwrMgmt;
	
	
	if (!gIsItg3205Initialized)
	{
		goto Exit;
	}
	
	Status = ReadItg3205Register(ADR_PWR_MGMT, &PwrMgmt, sizeof(ITG3205_PWR_MGMT));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Power Register. \r\n");
		
		goto Exit;
	}
	
	PwrMgmt.Sleep = Enable;
	
	Status = SetItg3205Register(ADR_PWR_MGMT, &PwrMgmt, sizeof(ITG3205_PWR_MGMT));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program Power Register. \r\n");
		
		goto Exit;
	}
	
Exit:

	return Status;
}

bool EnableDisableItg3205Standby(bool StbyXGEnable, bool StbyYGEnable, bool StbyZGEnable)
{
	bool Status = FALSE;
	
	ITG3205_PWR_MGMT PwrMgmt;
	
	
	if (!gIsItg3205Initialized)
	{
		goto Exit;
	}
	
	Status = ReadItg3205Register(ADR_PWR_MGMT, &PwrMgmt, sizeof(ITG3205_PWR_MGMT));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Power Register. \r\n");
		
		goto Exit;
	}
	
	PwrMgmt.StbyXG = StbyXGEnable;
	
	PwrMgmt.StbyYG = StbyYGEnable;
	
	PwrMgmt.StbyZG = StbyZGEnable;
	
	Status = SetItg3205Register(ADR_PWR_MGMT, &PwrMgmt, sizeof(ITG3205_PWR_MGMT));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program Power Register. \r\n");
		
		goto Exit;
	}
	
	Exit:

	return Status;
}

bool ReadItg3205GyroData(PGYRO_DATA pGyroData)
{
	bool Status = FALSE;
	
	ITG3205_INTERRUPT_STATUS InterruptStatus;
	
	char Itg3205Data[8];
	
	if (!gIsItg3205Initialized)
	{
		goto Exit;
	}
	
	Status = ReadItg3205Register(ADR_INTERRUPT_STATUS, &InterruptStatus, sizeof(ITG3205_INTERRUPT_STATUS));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Interrupt Status register.\r\n");
		
		goto Exit;
	}
	
	if (InterruptStatus.RawDataReady)
	{
	
		Status = ReadItg3205Register(ADR_TEMP_OUT_H, &Itg3205Data[0], 8);
	
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to read Gyro Data registers. \r\n");
		
			goto Exit;
		}
	
		pGyroData->Temperature = (Itg3205Data[0] << 8) | Itg3205Data[1];
		
		pGyroData->Temperature = 35 + ((pGyroData->Temperature + ITG3205_TEMP_OFFSET) / ITG3205_TEMP_SCALE_FACTOR);
	
		pGyroData->GyroXAxis = ((Itg3205Data[2] << 8) | Itg3205Data[3]) / ITG3205_GYRO_SCALE_FACTOR;
	
		pGyroData->GyroYAxis = ((Itg3205Data[4] << 8) | Itg3205Data[5]) / ITG3205_GYRO_SCALE_FACTOR;
	
		pGyroData->GyroZAxis = ((Itg3205Data[6] << 8) | Itg3205Data[7]) / ITG3205_GYRO_SCALE_FACTOR;
	}
	else
	{
		Status = FALSE;
	}
	
Exit:

	return Status;
}

//
// Private Functions
//

bool ConfigureItg3205DigitalLowPassFilter(ITG3205_DLPF_CFG DlpfCfg)
{
	bool Status = FALSE;
	
	ITG3205_DLPF_REG DlpfReg;
	
	
	
	DlpfReg.DlpfCfg = DlpfCfg;
	
	DlpfReg.FsSel = FS_SEL;
	
	Status = SetItg3205Register(ADR_DLPF, &DlpfReg, sizeof(ITG3205_DLPF_REG));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program DLPF Register. \r\n");
	}

	return Status;
}

bool ConfigureItg3205SampleRateDivider(unsigned char SmplrtDiv)
{
	bool Status = FALSE;
	
	
	Status = SetItg3205Register(ADR_SAMPLE_RATE_DIVIDER, &SmplrtDiv, sizeof(unsigned char));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program Sample Rate Divider Register. \r\n");
	}

	return Status;
}

bool ConfigureItg3205Clock(ITG3205_CLK_SEL ClkSel)
{
	bool Status = FALSE;
	
	ITG3205_PWR_MGMT PwrMgmt;
	
	
	PwrMgmt.AsUINT8 = 0;
	
	PwrMgmt.ClkSel = ClkSel;
	
	Status = SetItg3205Register(ADR_PWR_MGMT, &PwrMgmt, sizeof(ITG3205_PWR_MGMT));
	
	if(!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to program Power Register. \r\n");
	}

	return Status;
}

bool CheckItg3205Id()
{
	ITG3205_WHO_AM_I WhoAmI;
	
	bool Status;
	
	Status = ReadItg3205Register(ADR_WHO_AM_I, &WhoAmI, sizeof(ITG3205_WHO_AM_I));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Who Am I register.\r\n");
		
		goto Exit;
	}
	
	if (WhoAmI.ID != WHO_AM_I_ID)
	{
		SendTrace(ERROR_LEVEL, "ITG3205 device ID check failed - 0x%x.\r\n", WhoAmI.ID);
		
		Status = FALSE;
	}
	
Exit:

	return Status;
}

bool PrintItg3205ConfigRegisters()
{
	
	bool Status;
	
	ITG3205_DLPF_REG DlpfReg;
	
	ITG3205_PWR_MGMT PwrMgmt;
	
	ITG3205_INTERRUPT_CONFIG InterruptConfig;
	
	unsigned char SampleRateDivider;
	
	Status = ReadItg3205Register(ADR_DLPF, &DlpfReg.AsUINT8, sizeof(ITG3205_DLPF_REG));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read DLPF Register.\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "DLPF Reg = 0x%x\r\n", DlpfReg.AsUINT8);
	
	Status = ReadItg3205Register(ADR_PWR_MGMT, &PwrMgmt.AsUINT8, sizeof(ITG3205_PWR_MGMT));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Pwr Mgmt Register.\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Pwr Mgmt Reg = 0x%x\r\n", PwrMgmt.AsUINT8);
	
	Status = ReadItg3205Register(ADR_INTERRUPT_CONFIG, &InterruptConfig.AsUINT8, sizeof(ITG3205_INTERRUPT_CONFIG));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Interrupt Config Register.\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Interrupt Config Reg = 0x%x\r\n", InterruptConfig.AsUINT8);
	
	Status = ReadItg3205Register(ADR_SAMPLE_RATE_DIVIDER, &SampleRateDivider, sizeof(unsigned char));
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Sample Rate Divider Register.\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Sample Rate Divider Reg = 0x%x\r\n", SampleRateDivider);
	
Exit:

	return Status;
}

bool ReadItg3205Register(unsigned char RegisterAddress, PVOID pRegisterValue, unsigned long Length)
{
	
	bool Status = FALSE;
	
	Status = TwiTransmit(&RegisterAddress, sizeof (unsigned char), &gItg3205DeviceAddress, FALSE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to transmit Register Address. \r\n");
		
		goto Exit;
	}
	
	Status = TwiReceive(pRegisterValue, Length, &gItg3205DeviceAddress, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to retrieve Register Value. \r\n");
	}
	
	Exit:

	return Status;
}

bool SetItg3205Register(unsigned char RegisterAddress, PVOID pRegisterValue, unsigned long Length)
{	
	bool Status;
	
	unsigned char TwiData[Length + 1];
	
	unsigned long index;
	
	
	TwiData[0] = RegisterAddress;
	
	for (index = 0; index < Length; index ++)
	{
		TwiData[index + 1] = *((unsigned char *)pRegisterValue + index);
	}
	
	Status = TwiTransmit(&TwiData[0], Length + 1, &gItg3205DeviceAddress, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to transmit Register Address. \r\n");
	}

	return Status;
}

