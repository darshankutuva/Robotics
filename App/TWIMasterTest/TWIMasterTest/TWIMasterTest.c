/*
 * TWIMasterTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 


#include <avr/io.h>

#include "Trace.h"

#include "TWI.h"

#define SLAVE_ADDRESS	0x27

int main(void)
{
	unsigned int IndexI;
	
	unsigned int Value;
	
	TWI_ADDRESS SlaveAddress;
	
	//
	// Init Tracing
	//
	
	InitTrace(F_CPU, ERROR_LEVEL | INFO_LEVEL);
	
	SendTrace(INFO_LEVEL, "TWI Master Test\r\n");
	
	SendTrace(INFO_LEVEL, "---------------\r\n");
	
	//
	// Initialize TWI in Master Mode
	//
	
	InitTWI(TWI_MASTER, NULL, NULL);
	
	SlaveAddress.Address = SLAVE_ADDRESS;
	
	
	
	for (IndexI = 0; IndexI < 1000; IndexI++)
	{
		TwiTransmit(&IndexI, sizeof(unsigned int), &SlaveAddress, FALSE);
			
		SendTrace(INFO_LEVEL, "Data Transmitted = 0x%x\r\n", IndexI);
			
		TwiReceive(&Value, sizeof(unsigned int), &SlaveAddress, TRUE);
			
		SendTrace(INFO_LEVEL, "Data Received = 0x%x\r\n", Value);	
	}
}