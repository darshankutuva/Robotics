/*
 * HMC5883L.h
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


#ifndef HMC5883L_H_
#define HMC5883L_H_

#include "Common.h"

typedef enum _SAMPLES_AVERAGED_PER_MEASUREMENT
{
	ONE_SAMPLE		= 0x0,
	
	TWO_SAMPLES		= 0x1,
	
	FOUR_SAMPLES	= 0x2,
	
	EIGHT_SAMPLES	= 0x3
	
} SAMPLES_AVERAGED_PER_MEASUREMENT;

typedef enum _DATA_OUTPUT_RATE
{
	POINT_SEVEN_FIVE_HZ		= 0x0,
	
	ONE_POINT_FIVE_HZ		= 0x1,
	
	THREE_HZ				= 0x2,
	
	SEVEN_POINT_FIVE_HZ		= 0x3,
	
	FIFTEEN_HZ				= 0x4,
	
	THIRTY_HZ				= 0x5,
	
	SEVENTY_FIVE_HZ			= 0x6
	
} DATA_OUTPUT_RATE;

typedef enum _MEASUREMENT_MODE
{
	NORMAL					= 0x0,
		
	POSITIVE_BIAS			= 0x1,
	
	NEGATIVE_BIAS			= 0x2

} MEASUREMENT_MODE;

typedef enum _GAIN_CONFIGURATION
{
	THOUSAND_THREE_SEVENTY	= 0x0,
	
	THOUSAND_AND_NINTY		= 0x1,
	
	EIGHT_TWENTY			= 0x2,
	
	SIX_SIXTY				= 0x3,
	
	FOUR_FORTY				= 0x4,
	
	THREE_NINTY				= 0x5,
	
	THREE_THIRTY			= 0x6,
	
	TWO_THIRTY				= 0x7
	
} GAIN_CONFIGURATION;

typedef enum _OP_MODE
{
	CONTINUOUS_MEASUREMENT_MODE		= 0x0,
	
	SINGLE_MEASUREMENT_MODE			= 0x1,
	
	IDLE_MODE						= 0x2
		
} OP_MODE;

typedef struct _MAGNETOMETER_DATA
{
	short RawDataX;
	
	short RawDataY;
	
	short RawDataZ;
	
	double GaussX;
	
	double GaussY;
	
	double GaussZ;
	
} MAGNETOMETER_DATA, *PMAGNETOMETER_DATA;

//
// Public Functions
//

bool InitHMC5883LMagnetometer(OP_MODE OpMode,
					  MEASUREMENT_MODE MeasurementMode,
					  DATA_OUTPUT_RATE DataOutputRate,
					  SAMPLES_AVERAGED_PER_MEASUREMENT SamplesAveragedPerMeasurement,
					  GAIN_CONFIGURATION GainConfiguration);
					  
bool ReadHMC5883LContinuousMeasurementData(PMAGNETOMETER_DATA pData);

bool ReadHMC5883LSingleMeasurementData(PMAGNETOMETER_DATA pData);

bool CalculateCompassHeading(PMAGNETOMETER_DATA pMagData, double Theta, double Phi, ANGLE_UNIT AngleUnit, double *pCompassHeading);


#endif /* HMC5883L_H_ */