/*
 * ITG3205_Internal.h
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


#ifndef ITG3205_INTERNAL_H_
#define ITG3205_INTERNAL_H_

#include "Common.h"

//
// Device Address
//

#define DEVICE_ITG3205_ADDRESS_GND	0x68

#define DEVICE_ITG3205_ADDRESS_HIGH	0x69

#define GET_ITG3205_DEVICE_ADDRESS(IsAltAddressGnd) ((IsPin9Gnd) ? (DEVICE_ITG3205_ADDRESS_GND) : (DEVICE_ITG3205_ADDRESS_HIGH))

//
// Who AM I
//

#define ADR_WHO_AM_I				0x0

#define WHO_AM_I_ID					0x34

typedef struct _ITG3205_WHO_AM_I
{
	unsigned char Rsvd0 : 1;
	
	unsigned char ID : 6;
	
	unsigned char Rsvd1 : 1;
		
} ITG3205_WHO_AM_I, *PITG3205_WHO_AM_I;

//
// Sample Rate Divider
//

#define ADR_SAMPLE_RATE_DIVIDER		0x15

//
// DLPF
//

#define ADR_DLPF					0x16

#define FS_SEL						0x3

typedef union _ITG3205_DLPF_REG
{
	struct  
	{
		unsigned char DlpfCfg : 3;
		
		unsigned char FsSel : 2;
		
		unsigned char Rsvd : 3;
	};
	
	unsigned char AsUINT8;
	
} ITG3205_DLPF_REG, *PITG3205_DLPF_REG;

//
// Interrupt Configuration
//

#define ADR_INTERRUPT_CONFIG		0x17

//
// Interrupt Status
//

#define ADR_INTERRUPT_STATUS		0x1A

typedef union _INTERRUPT_STATUS
{
	struct  
	{
		unsigned char RawDataReady : 1;
		
		unsigned char Rsvd0 : 1;
		
		unsigned char ItgReady : 1;
		
		unsigned char Rsvd1 : 5;
	};
	
	unsigned char AsUINT8;
		
} ITG3205_INTERRUPT_STATUS, *PITG3205_INTERRUPT_STATUS;

#define ITG3205_TEMP_OFFSET			13200

#define ITG3205_TEMP_SCALE_FACTOR	280

#define ITG3205_GYRO_SCALE_FACTOR	14.375

//
// Temp Out High
//

#define ADR_TEMP_OUT_H				0x1B

//
// Temp Out Low
//

#define ADR_TEMP_OUT_L				0x1C

//
// GYRO X Out High
//

#define ADR_GYRO_XOUT_H				0x1D

//
// GYRO X Out Low
//

#define ADR_GYRO_XOUT_L				0x1E

//
// GYRO Y Out High
//

#define ADR_GYRO_YOUT_H				0x1F

//
// GYRO Y Out Low
//

#define ADR_GYRO_YOUT_L				0x20

//
// GYRO Z Out High
//

#define ADR_GYRO_ZOUT_H				0x21

//
// GYRO Z Out Low
//

#define ADR_GYRO_ZOUT_L				0x22

//
// Power Management
//

#define ADR_PWR_MGMT				0x3E

typedef union _ITG3205_PWR_MGMT
{
	struct  
	{
		unsigned char ClkSel : 3;
		
		unsigned char StbyZG : 1;
		
		unsigned char StbyYG : 1;
		
		unsigned char StbyXG : 1;
		
		unsigned char Sleep  : 1;
		
		unsigned char HReset : 1;
	};
	
	unsigned char AsUINT8;
		
} ITG3205_PWR_MGMT, *PITG3205_PWR_MGMT;

//
// Private Functions
//

bool ConfigureItg3205DigitalLowPassFilter(ITG3205_DLPF_CFG DlpfCfg);

bool ConfigureItg3205SampleRateDivider(unsigned char SmplrtDiv);

bool ConfigureItg3205Clock(ITG3205_CLK_SEL ClkSel);

bool CheckItg3205Id();

bool ConfigureItg3205Interrupt(PITG3205_INTERRUPT_CONFIG pInterruptConfig);

bool PrintItg3205ConfigRegisters();

bool ReadItg3205Register(unsigned char RegisterAddress, PVOID pRegisterValue, unsigned long Length);

bool SetItg3205Register(unsigned char RegisterAddress, PVOID pRegisterValue, unsigned long Length);

#endif /* ITG3205_H_ */