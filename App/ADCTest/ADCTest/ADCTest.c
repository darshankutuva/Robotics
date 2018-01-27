/*
 * ADCTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 


#include "Common.h"

#include "ADC.h"

#include "Trace.h"

int main(void)
{
	ADC_INIT_PARAMS AdcParams;
	
	unsigned int Value;
	
	unsigned int XValue;
	
	unsigned int YValue;
	
	//
	// Initialize Tracing
	//
	
	InitTrace(F_CPU, ERROR_LEVEL | INFO_LEVEL);
	
	AdcParams.Flags.IsLeftAdjustResult = 0;
	
	AdcParams.RefVoltSelect = AREF_VOLT;
	
	AdcParams.Prescalar = DIV_FACTOR_0;
	
	AdcParams.EnabledChannelsBitVector = (0x1 << ADC_CHANNEL_0) | (0x1 << ADC_CHANNEL_1) | (0x1 << ADC_CHANNEL_2);
	
	InitADC(&AdcParams);
	
	while (1)
	{
		Value = 0;
		
		XValue = 0;
		
		YValue = 0;
		
		ADCRead(ADC_CHANNEL_0, &Value);
		
		ADCRead(ADC_CHANNEL_0, &Value);
		
		ADCRead(ADC_CHANNEL_1, &XValue);
		
		ADCRead(ADC_CHANNEL_1, &XValue);
		
		ADCRead(ADC_CHANNEL_2, &YValue);
		
		ADCRead(ADC_CHANNEL_2, &YValue);
		
		SendTrace(INFO_LEVEL, "ADC P = %d, X = %d, Y = %d\r\n", Value, XValue, YValue);
	}
}