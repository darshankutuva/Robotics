/*
 * ADXL345_Internal.h
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


#ifndef ADXL345_Internal_H_
#define ADXL345_Internal_H_

#include "Common.h"

//
// Device Address
//

#define DEVICE_ADDRESS_GND	0x53

#define DEVICE_ADDRESS_HIGH	0x1D

#define GET_DEVICE_ADDRESS(IsAltAddressGnd) ((IsAltAddressGnd) ? (DEVICE_ADDRESS_GND) : (DEVICE_ADDRESS_HIGH)) 

//
// DEVID Register
//

#define ADR_DEVID	0x0
#define DEVID		0xE5

//
// OFSX, OFSY, OFSZ Register
//

#define ADR_OFSX	0x1E
#define ADR_OFSY	0x1F
#define ADR_OFSZ	0x20

//
// LSB Per G for Full Resolution
// Note: This library programs ADXL345 to full resolution
//

#define LsbPerG		256.0

//
// Duration Register
//

#define ADR_DUR		0x21

//
// Latent Register
//

#define ADR_Latent	0x22

//
// Window Register
//

#define ADR_Window	0x23

//
// THRESH_ACT Register
//

#define ADR_THRESH_ACT 0x24

//
// THRESH_INACT Register
//

#define ADR_THRESH_INACT  0x25

//
// TIME_INACT
//

#define ADR_TIME_INACT	0x26

//
// ACT_INACT_CTL
//

#define ADR_INACT_CTL	0x27

//
// THRESH_FF Register
//

#define ADR_THRESH_FF	0x28

//
// TIME_FF Register
//

#define ADR_TIME_FF		0x29

//
// TAP_AXES Register
//

#define ADR_TAP_AXES	0x2A

//
// ACT_TAP_STATUS Register
//

#define ADR_ACT_TAP_STATUS  0x2B

//
// BW_RATE Register
//

#define ADR_BW_RATE		0x2C

typedef union _BW_RATE
{
	struct
	{
		unsigned char Rate		: 4;
	
		unsigned char LowPwr	: 1;
	
		unsigned char Rsvd		: 3;
	};
	
	unsigned char AsUINT8;
	
} BW_RATE, *PBW_RATE;

//
// POWER_CTL Register 
//

#define ADR_POWER_CTL	0x2D

typedef union _POWER_CTL
{
	struct  
	{
		unsigned char Wakeup	: 2;
	
		unsigned char Sleep		: 1;
	
		unsigned char Measure	: 1;
	
		unsigned char AutoSleep	: 1;
	
		unsigned char Link		: 1;
	
		unsigned char Rsvd		: 2;
		
	};
	
	unsigned char AsUINT8;

} POWER_CTL, *PPOWER_CTL;

//
// INTERRUPT Registers
//

#define ADR_INT_ENABLE	0x2E

#define ADR_INT_MAP		0x2F

#define ADR_INT_SOURCE	0x30

//
// Data Format Register
//

#define ADR_DATA_FORMAT		0x31

typedef union _DATA_FORMAT
{
	struct
	{
		unsigned char Range		: 2;
	
		unsigned char Justify	: 1;
	
		unsigned char FullRes	: 1;
	
		unsigned char Rsvd		: 1;
	
		unsigned char IntInvert	: 1;
	
		unsigned char SPI		: 1;
	
		unsigned char SelfTest	: 1;
	};
	
	unsigned char AsUINT8;
	
} DATA_FORMAT, *PDATA_FORMAT;

//
// Data Registers
//

#define ADR_DATAX0	0x32

#define ADR_DATAX1	0x33

#define ADR_DATAY0	0x34

#define ADR_DATAY1	0x35

#define ADR_DATAZ0	0x36

#define ADR_DATAZ1	0x37

//
// FIFO_CTL Register
//

#define ADR_FIFO_CTL	0x38

//
// FIFO_STATUS Register
//

#define ADR_FIFO_STATUS	0x39

//
// Private Functions
//

bool CheckADXL345DeviceId();

bool ReadADXL345Register(unsigned char RegisterAddress, PVOID RegisterValue, unsigned long Length);

bool SetADXL345Register(unsigned char RegisterAddress, PVOID RegisterValue, unsigned long Length);

#endif /* ADXL345_H_ */