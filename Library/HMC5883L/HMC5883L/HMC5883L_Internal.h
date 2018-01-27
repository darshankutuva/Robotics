/*
 * HMC5883L_Internal.h
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


#ifndef HMC5883L_INTERNAL_H_
#define HMC5883L_INTERNAL_H_

#define DEVICE_ADDRESS		0x1E

#define HMC5883L_SCALE_FACTOR	0.48828125

//
// Configuration Register A
//

#define ADR_CONFIG_REG_A		0x0

typedef union _CONFIG_REG_A
{
	struct
	{
		unsigned char MeasurementMode : 2;
		
		unsigned char DataOutputRate : 3;
		
		unsigned char SamplesAveragedPerMeasurement : 2;
		
		unsigned char StartStopMeasurement : 1;
	};
	
	unsigned char AsUINT8;
	
} CONFIG_REG_A, *PCONFIG_REG_A;

typedef union _CONFIG_REG_B
{
	struct  
	{
		unsigned char Rsvd : 5;
		
		unsigned char GainConfiguration : 3;
	};
	
	unsigned char AsUINT8;

} CONFIG_REG_B, *PCONFIG_REG_B;

typedef union _MODE_REG
{
	struct
	{
		unsigned char Mode : 2;
		
		unsigned char Rsvd : 5;
		
		unsigned char SingleMeasurementStatus : 1;		
	};
	
	unsigned char AsUINT8;
		
} MODE_REG, *PMODE_REG;

typedef union _STATUS_REG
{
	struct  
	{
		unsigned char Ready : 1;
		
		unsigned char Lock :  1;
		
		unsigned char Rsvd :  6;
	};
	
	unsigned char AsUINT8;
		
} STATUS_REG, *PSTATUS_REG;

//
// Configuration Register B
//

#define ADR_CONFIG_REG_B		0x1

//
// Mode Register
//

#define ADR_MODE_REG			0x2

//
// Data Output X MSB Register
//

#define ADR_DATA_OUT_X_MSB_REG	0x3

//
// Data Output X LSB Register
//

#define ADR_DATA_OUT_X_LSB_REG	0x4

//
// Data Output Z MSB Register
//

#define ADR_DATA_OUT_Z_MSB_REG	0x5

//
// Data Output Z LSB Register
//

#define ADR_DATA_OUT_Z_LSB_REG	0x6

//
// Data Output Y MSB Register
//

#define ADR_DATA_OUT_Y_MSB_REG	0x7

//
// Data Output Y LSB Register
//

#define ADR_DATA_OUT_Y_LSB_REG	0x8

//
// Status Register
//

#define ADR_STATUS_REG			0x9

//
// ID Reg A
//

#define ADR_ID_REG_A			0xA

#define ID_A					0x48

//
// ID Reg B
//

#define ADR_ID_REG_B			0xB

#define ID_B					0x34

//
// ID Reg C
//

#define ADR_ID_REG_C			0xC

#define ID_C					0x33


//
// Private Functions
//

bool IsHMC5883LDataReady();

bool ReadHMC5883LData(PMAGNETOMETER_DATA pDATA);

bool ReadHMC5883LRegister(unsigned char RegisterAddress, PVOID pRegisterValue, unsigned long Length);

bool SetHMC5883LRegister(unsigned char RegisterAddress, PVOID pRegisterValue, unsigned long Length);

bool CheckHMC5883LDeviceId();

bool PrintHMC5883LConfigRegisters();

#endif /* HMC5883L_H_ */