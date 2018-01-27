/*
 * GyroscopeTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 


#include "Common.h"

#include "Trace.h"

#include "Twi.h"

#include "ITG3205.h"

int main(void)
{
	bool Status;
	
	ITG3205_INTERRUPT_CONFIG InterruptConfig;
	
	GYRO_DATA GyroData;
	
	
	//
	// Initialize Tracing
	//
	
	InitTrace(F_CPU, ERROR_LEVEL | INFO_LEVEL);
	
	//
	// Initialize TWI
	//

	if(!InitTWI(TWI_MASTER, NULL, NULL))
	{
		goto Exit;
	}
	
	
	InterruptConfig.AsUINT8 = 0;
	
	InterruptConfig.RawDataReadyEn = 1;
	
	InterruptConfig.ItgReadyEn = 1;
	
	InterruptConfig.LatchIntEn = 1;
	
	InterruptConfig.ActiveLowEn = 1;
	
	
	Status = InitItg3205(TWO_FIVE_SIX_HZ, 0, &InterruptConfig, INTERNAL_OSCILLATOR, TRUE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to initialize ITG3205 Gyro.\r\n");
		
		goto Exit;
	}
	
	SendTrace(INFO_LEVEL, "Gyroscope Initialized.\r\n");
	
    while(1)
    {
        Status = ReadItg3205GyroData(&GyroData);
		
		if (Status)
		{
			SendTrace(INFO_LEVEL, "Temperature = %f degrees C.\r\n", GyroData.Temperature);
			
			SendTrace(INFO_LEVEL, "Gyro X Axis = %f degrees per second.\r\n", GyroData.GyroXAxis);

			SendTrace(INFO_LEVEL, "Gyro Y Axis = %f degrees per second.\r\n", GyroData.GyroYAxis);
			
			SendTrace(INFO_LEVEL, "Gyro Z Axis = %f degrees per second.\r\n", GyroData.GyroZAxis);			
				
		}
    }
	
Exit:

	return 1;
}