/*
 * ADXL345.h
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


#ifndef ADXL345_H_
#define ADXL345_H_

#include "Common.h"

//
// ACT_INACT_CTL
//

typedef union _ACT_INACT_CTL
{
	struct  
	{
		unsigned char InactZEnable  : 1;
		
		unsigned char InactYEnable  : 1;
		
		unsigned char InactXEnable  : 1;
		
		unsigned char InactAcDc		: 1;
		
		unsigned char ActZEnable	: 1;
		
		unsigned char ActYEnable	: 1;
		
		unsigned char ActXEnable	: 1;
		
		unsigned char ActAcDc		: 1;
	};
	
	unsigned char AsUINT8;
	
} ACT_INACT_CTL, *PACT_INACT_CTL;

//
// TAP_AXES Register
//

typedef union _TAP_AXES
{
	struct  
	{
		unsigned char TapZEnable : 1;
	
		unsigned char TapYEnable : 1;
	
		unsigned char TapXEnable : 1;
	
		unsigned char Suppress	 : 1;
	
		unsigned char Rsvd		 : 4;
	
	};
	
	unsigned char AsUINT8;
	
} TAP_AXES, *PTAP_AXES;

//
// ACT_TAP_STATUS Register
//

typedef union _ACT_TAP_STATUS
{
	struct  
	{
		unsigned char TapZSource : 1;
	
		unsigned char TapYSource : 1;
	
		unsigned char TapXSource : 1;
	
		unsigned char ASleep     : 1;
	
		unsigned char ActZSource : 1;
	
		unsigned char ActYSource : 1;
	
		unsigned char ActXSource : 1;
	
		unsigned char Rsvd		 : 1;
	};
	
	unsigned char AsUINT8;
	
} ACT_TAP_STATUS, *PACT_TAP_STATUS;

//
// POWER_CTL Register
//

typedef enum _WAKEUP_FREQ
{
	EIGHT_HZ	= 0x0,
	
	FOUR_HZ		= 0x1,
	
	TWO_HZ		= 0x2,
	
	ONE_HZ		= 0x3,
	
	NO_WAKEUP	= 0x4
	
} WAKEUP_FREQ;

//
// INTERRUPT Registers
//

typedef union _INT_ENABLE_MAP_SOURCE
{
	struct  
	{
		unsigned char Overrun		: 1;
	
		unsigned char Watermark		: 1;
	
		unsigned char FreeFall		: 1;
	
		unsigned char Inactivity	: 1;
	
		unsigned char Activity		: 1;
	
		unsigned char DoubleTap		: 1;
	
		unsigned char SingleTap		: 1;
	
		unsigned char DataReady		: 1;
	};
	
	unsigned char AsUINT8;

} INT_ENABLE_MAP_SOURCE, *PINT_ENABLE_MAP_SOURCE;

typedef enum _DATA_RANGE
{
	TWO_G		= 0x0,
	
	FOUR_G		= 0x1,
	
	EIGHT_G		= 0x2,
	
	SIXTEEN_G	= 0x3
	
} DATA_RANGE;

typedef enum _DATA_RATE
{
	POINT_ONE_HZ						= 0x0,
	
	POINT_TWO_HZ						= 0x1,
	
	POINT_THREE_NINE_HZ					= 0x2,
	
	POINT_SEVEN_EIGHT_HZ				= 0x3,
	
	ONE_POINT_FIVE_SIX_HZ				= 0x4,
	
	THREE_POINT_ONE_THREE_HZ			= 0x5,
	
	SIX_POINT_TWO_FIVE_HZ				= 0x6,
	
	TWELVE_POINT_FIVE_HZ				= 0x7,
	
	TWENTY_FIVE_HZ						= 0x8,
	
	FIFTY_HZ							= 0x9,
	
	HUNDRED_HZ							= 0xA,
	
	TWO_HUNDRED_HZ						= 0xB,
	
	FOUR_HUNDRED_HZ						= 0xC,
	
	EIGHT_HUNDRED_HZ					= 0xD,
	
	THOUSAND_SIX_HUNDRED_HZ				= 0xE,
	
	THREE_THOUSAND_TWO_HUNDRED_HZ		= 0xF
	
} DATA_RATE;

typedef struct _DATA_OFFSET
{
	char OffsetX;
	
	char OffsetY;
	
	char OffsetZ;	
	
} DATA_OFFSET, *PDATA_OFFSET;

//
// Data Registers
//

typedef struct _RAW_DATA_XYZ
{
	short DataX;
	
	short DataY;
	
	short DataZ;		
	
} RAW_DATA_XYZ, *PRAW_DATA_XYZ;

typedef struct _ACC_TILT_DATA_XYZ
{
	double AccX;
	
	double AccY;
	
	double AccZ;
	
	double TiltX;
	
	double TiltY;
	
	double TiltZ;
	
} ACC_TILT_DATA_XYZ, *PACC_TILT_DATA_XYZ;

//
// AXES ENUM
//

typedef enum _AXES_XYZ
{
	AXES_X	= 0,
	
	AXES_Y	= 1,
	
	AXES_Z	= 2	

} AXES_XYZ;

//
// FIFO CTL Register
//

typedef union _FIFO_CTL
{
	struct  
	{
		unsigned char Samples   : 5;
	
		unsigned char Trigger   : 1;
	
		unsigned char FIFOMode	: 2;
	};
	
	unsigned char AsUINT8;
	
} FIFO_CTL, *PFIFO_CTL;

typedef enum _FIFO_MODE
{
	BYPASS		= 0x0,
	
	FIFO		= 0x1,
	
	STREAM		= 0x2,
	
	TRIGGER		= 0x3
	
} FIFO_MODE;

//
// FIFO STATUS Register
//

typedef union _FIFO_STATUS
{
	struct  
	{
		unsigned char Entries	: 6;
	
		unsigned char Rsvd		: 1;
	
		unsigned char FIFOTrig	: 1;
	};
	
	unsigned char AsUINT8;

} FIFO_STATUS, *PFIFO_STATUS;

//
// Power Mode
//

typedef enum _POWER_MODE
{
	LOW_POWER			= 0,
	
	AUTO_SLEEP			= 1,
	
	FULL_POWER			= 2
		
} POWER_MODE;

//
// Functions
//

bool InitADXL345DevAddress(bool IsAltAddressGnd);

bool EnableDisableADXL345StandbyMode(bool Enable);

bool ProgramADXL345PowerCtl(POWER_MODE PowerMode, WAKEUP_FREQ WakeupFreq);

bool ProgramADXL345Interrupts(PINT_ENABLE_MAP_SOURCE IntMap, PINT_ENABLE_MAP_SOURCE IntEnable);

bool ProgramADXL345FifoControl(PFIFO_CTL FifoCtl);

bool ProgramADXL345DataFormat(DATA_RANGE DataRange, bool IntInvert, PDATA_OFFSET DataOffset, DATA_RATE DataRate);

bool ProgramADXL345ActivityInactivityDetection(PACT_INACT_CTL ActInactCtl, unsigned char ActThreshold, unsigned char InactThreshold, unsigned char TimeInact);

bool ProgramADXL345FreeFallDetection(unsigned char FreeFallThreshold, unsigned char FreeFallTime);

bool ProgramADXL345TapDetection(PTAP_AXES TapAxes, unsigned char Duration, unsigned char Latency, unsigned char Window);

bool ReadADXL345InterruptSource(PINT_ENABLE_MAP_SOURCE IntSource);

bool ReadADXL345FifoStatus(PFIFO_STATUS FifoStatus);

bool ReadADXL345ActTapStatus(PACT_TAP_STATUS ActTapStatus);

bool ReadADXL345RawData(PRAW_DATA_XYZ RawData);

bool ReadADXL345AccAndTiltData(ANGLE_UNIT AngleUnit, PACC_TILT_DATA_XYZ AccTiltData);

bool FlushADXL345Fifo();

#endif /* ADXL345_H_ */