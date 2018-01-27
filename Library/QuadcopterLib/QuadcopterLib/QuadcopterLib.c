/*
 * QuadcopterLib.c
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

#include "Common.h"

#include "Trace.h"

#include "Transceiver.h"

#include "PWM.h"

#include "ADC.h"

#include "ADXL345.h"


#define MAX_TILT				4

#define TH_SCALE_FACTOR			(ADC_MAX_VALUE / (PWM_PULSE_DIVS - MAX_TILT))

//
// Global Definition
//
																 
double gAccX = 0;

double gAccY = 0;

double gThPulse;



void
CalculatePulse(unsigned int ThValue, unsigned int XValue, unsigned int YValue)
{	
	double XPulse;
	
	double YPulse;
	
	double XTilt;
	
	double YTilt;
	
	ACC_TILT_DATA_XYZ AccTiltData;
	
	bool Status;
	
	//
	// Calculate Pulse
	//
	
	gThPulse = ThValue / (double)TH_SCALE_FACTOR;
	
	//
	// Scale XPulse and YPulse to -1 to 1.
	//
	
	XPulse = ((XValue - ADC_CENTER) * 2.0) / ADC_MAX_VALUE;
	
	YPulse = ((YValue - ADC_CENTER) * 2.0) / ADC_MAX_VALUE;
	
	//
	// Scale XPulse and YPulse to current Thrust scale
	//
	
	XPulse = XPulse * (gThPulse / PWM_PULSE_DIVS);
	
	YPulse = YPulse * (gThPulse / PWM_PULSE_DIVS);

	//
	// Read Accelerometer Data
	//
	
	Status = ReadADXL345AccAndTiltData(DEGREES, &AccTiltData);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to read Accelerometer Data.\r\n");
			
		AccTiltData.AccX = gAccX;
		
		AccTiltData.AccY = gAccY;
	}
	else
	{
		gAccX = AccTiltData.AccX;
		
		gAccY = AccTiltData.AccY;
	}
	
	//
	// Scale Accelerometer data to current Thrust scale
	//
	
	 AccTiltData.AccX =  (AccTiltData.AccX / 2.0) * (gThPulse / PWM_PULSE_DIVS);
	 
	 AccTiltData.AccY =  (AccTiltData.AccY / 2.0) * (gThPulse / PWM_PULSE_DIVS);
	
	//
	// Now calculate Tilt
	//
	
	XTilt = XPulse - AccTiltData.AccX;
	
	YTilt = YPulse - AccTiltData.AccY;
	
	SendTrace(INFO_LEVEL, "ThPulse = %f, XPulse = %f, YPulse = %f, AccX = %f, AccY = %f, XTilt = %f, YTilt = %f\r\n", gThPulse, XPulse, YPulse, AccTiltData.AccX, AccTiltData.AccY, XTilt, YTilt);
		
	//
	// Now set PWM Pulse for X and Y axis
	//
		
	PWMChangePulseDuration(PWM_CHANNEL_0, RoundLong(gThPulse + XTilt - YTilt));
		
	PWMChangePulseDuration(PWM_CHANNEL_1, RoundLong(gThPulse + XTilt + YTilt));
		
	PWMChangePulseDuration(PWM_CHANNEL_2, RoundLong(gThPulse - XTilt - YTilt));
		
	PWMChangePulseDuration(PWM_CHANNEL_3, RoundLong(gThPulse - XTilt + YTilt));
}

void
PerformEmergencyLanding()
{
	char Index;
	
	for (Index = gThPulse; Index > 0; Index--)
	{
		PWMChangePulseDuration(PWM_CHANNEL_0, Index);
		
		PWMChangePulseDuration(PWM_CHANNEL_1, Index);
		
		PWMChangePulseDuration(PWM_CHANNEL_2, Index);
		
		PWMChangePulseDuration(PWM_CHANNEL_3, Index);
		
		_delay_ms(100);
	}
	
	while(1);
	
	//TurnOffPWM();
}