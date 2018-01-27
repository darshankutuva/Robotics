/*
 * PWMTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 


#include "Common.h"

#include "Trace.h"

#include "PWM.h"

#include "ADC.h"

int main(void)
{
	ADC_INIT_PARAMS AdcParams;
	
	unsigned int ADCValue;
	
	unsigned char Threshold;
	
	unsigned char Pulse;
	
	//
	// Initialize Tracing
	//
	
	InitTrace(F_CPU, ERROR_LEVEL | INFO_LEVEL);
	
	SendTrace(INFO_LEVEL, "PWM Test\r\n");
	
	SendTrace(INFO_LEVEL, "---------\r\n");
	
	//
	// Enable interrupts
	//
	
	sei();
	
	//
	// Initialize ADC
	//
	
	AdcParams.Flags.IsLeftAdjustResult = 0;
	
	AdcParams.RefVoltSelect = AREF_VOLT;
	
	AdcParams.Prescalar = DIV_FACTOR_0;
	
	AdcParams.EnabledChannelsBitVector = (0x1 << ADC_CHANNEL_0);
	
	InitADC(&AdcParams);
	
	//
	// Read ADC Value and Initialize PWM
	//
	
	Threshold = ADC_MAX_VALUE / PWM_PULSE_DIVS;
	
	/*ADCValue = 0;
	
	ADCRead(ADC_CHANNEL_0, &ADCValue);
	
	ADCRead(ADC_CHANNEL_0, &ADCValue);
	
	SendTrace(INFO_LEVEL,"ADC Value = %d\r\n", ADCValue);
	
	Pulse = ADCValue / Threshold;
	
	if (Pulse >= PWM_PULSE_DIVS)
	{
		Pulse = PWM_PULSE_DIVS;
	}*/ 
	
	Pulse = 7;
	
	InitPWM(Pulse);
	
	_delay_ms(1000);
	
    while(1)
    {
		ADCValue = 0;
		
		ADCRead(ADC_CHANNEL_0, &ADCValue);
		
		ADCRead(ADC_CHANNEL_0, &ADCValue);
		
		Pulse = ADCValue / Threshold;
		
		if (Pulse >= PWM_PULSE_DIVS)
		{
			Pulse = PWM_PULSE_DIVS;
		}    
    
		SendTrace(INFO_LEVEL,"ADC Value = %d, Pulse = %d\r\n", ADCValue, Pulse);
	
		PWMChangePulseDuration(PWM_CHANNEL_0, Pulse);
		
		PWMChangePulseDuration(PWM_CHANNEL_1, Pulse);
		
		PWMChangePulseDuration(PWM_CHANNEL_2, Pulse);
		
		PWMChangePulseDuration(PWM_CHANNEL_3, Pulse);
	}
}