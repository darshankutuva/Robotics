/*
 * ITG3205.h
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


#ifndef ITG3205_H_
#define ITG3205_H_

#include "Common.h"

//
// DLPF CFG
//

typedef enum _ITG3205_DLPF_CFG
{
	TWO_FIVE_SIX_HZ			= 0x0,
	
	ONE_EIGHT_EIGHT_HZ		= 0x1,
	
	NINTY_EIGHT_HZ			= 0x2,
	
	FOURTY_TWO_HZ			= 0x3,
	
	TWENTY_HZ				= 0x4,
	
	TEN_HZ					= 0x5,
	
	FIVE_HZ					= 0x6

} ITG3205_DLPF_CFG;

//
// CLK SEL
//

typedef enum _ITG3205_CLK_SEL
{
	INTERNAL_OSCILLATOR			= 0x0,
	
	PLL_WITH_X_GYRO_REF			= 0x1,
	
	PLL_WITH_Y_GYRO_REF			= 0x2,
	
	PLL_WITH_Z_GYRO_REF			= 0x3,
	
	PLL_WITH_EXT_32_768_KHZ_REF	= 0x4,
	
	PLL_WITH_EXT_19_2_MHZ_REF	= 0x5
	
} ITG3205_CLK_SEL;

typedef union _ITG3205_INTERRUPT_CONFIG
{
	struct
	{
		unsigned char RawDataReadyEn : 1;
		
		unsigned char Rsvd0 : 1;
		
		unsigned char ItgReadyEn : 1;
		
		unsigned char Rsvd1 : 1;
		
		unsigned char IntAnyReadToClear : 1;
		
		unsigned char LatchIntEn : 1;
		
		unsigned char OpenDrainEn : 1;
		
		unsigned char ActiveLowEn : 1;
	};
	
	unsigned char AsUINT8;
	
} ITG3205_INTERRUPT_CONFIG, *PITG3205_INTERRUPT_CONFIG;

typedef struct _GYRO_DATA
{
	double	Temperature;
	
	double	GyroXAxis;
	
	double	GyroYAxis;
	
	double	GyroZAxis;
		
} GYRO_DATA, *PGYRO_DATA;

//
// Public Functions
//

bool InitItg3205(ITG3205_DLPF_CFG DlpfCfg,
				 unsigned char SmplrtDiv,
				 PITG3205_INTERRUPT_CONFIG pInterruptConfig,
				 ITG3205_CLK_SEL ClkSel,
				 bool IsPin9Gnd);

bool ResetItg3205();

bool EnableDisableItg3205Sleep(bool Enable);

bool EnableDisableItg3205Standby(bool StbyXGEnable, bool StbyYGEnable, bool StbyZGEnable);

bool ReadItg3205GyroData(PGYRO_DATA pGyroData);

#endif /* ITG3205_H_ */