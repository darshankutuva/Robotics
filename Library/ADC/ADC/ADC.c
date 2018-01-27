/*
 * ADC.c
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

#include "ADC_Internal.h"

#include "Trace.h"

bool gADCInitialized = FALSE;

bool
InitADC(PADC_INIT_PARAMS InitParams)
{
	ADC_MUX AdcMux;
	
	ADC_CTRL_STATUS_A AdcCtrlStatusA;
	
	//
	// Check to see if the init params is set to NULL
	//
	
	if (InitParams == NULL)
	{
		goto Exit;
	}
	
	//
	// Initialize ADC Mux register
	//
	
	AdcMux.AsUINT8 = 0;
	
	AdcMux.RefVoltSelect = InitParams->RefVoltSelect;
	
	AdcMux.Adlar = InitParams->Flags.IsLeftAdjustResult;
	
	ADMUX = AdcMux.AsUINT8;
	
	//
	// Initialize Control and Status A Register
	//
	
	AdcCtrlStatusA.AsUINT8 = 0;
	
	AdcCtrlStatusA.Prescalar = InitParams->Prescalar;
	
	AdcCtrlStatusA.ADCEnable = 1;
	
	ADCSRA = AdcCtrlStatusA.AsUINT8;
	
	//
	// Initialize Control and Status Register B
	//
	
	ADCSRB = 0;
	
	//
	// Initialize Digital Input Disable Register 0
	//
	
	DIDR0 = InitParams->EnabledChannelsBitVector;
	
	
	gADCInitialized = TRUE;
	
	Exit:

	return gADCInitialized;
}

bool
ADCRead(ADC_CHANNEL AdcChannel, unsigned int *Value)
{
	bool Status = TRUE;
	
	ADC_MUX AdcMux;
	
	//
	// Read DIDR0 register to check enabled channels
	//
	
	if (!(DIDR0 & (0x1 << AdcChannel)))
	{
		Status = FALSE;
		
		SendTrace(ERROR_LEVEL, "ADC Channel Not Enabled\r\n");
		
		goto Exit;
	}
	
	//
	// Write the ADC Channel in ADCMUX
	//
	
	AdcMux.AsUINT8 = ADMUX;
	
	AdcMux.Mux = AdcChannel;
	
	ADMUX = AdcMux.AsUINT8; 
	
	//
	// Start Conversion
	//
	
	START_CONVERSION();
	
	//
	// Wait for Conversion to complete
	//
	
	WAIT_FOR_CONVERSION();
	
	//
	// Read the data
	//
	
	*Value = ADCL;
	
	*Value |= (ADCH << 8);
	
	if (*Value >= ADC_MAX_VALUE)
	{
		*Value = ADC_MAX_VALUE - 1;
	}
	
Exit:

	return Status;
}