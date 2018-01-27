/*
 * TwiSlaveTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 


#include <avr/io.h>

#include "Trace.h"

#include "TWI.h"

#define MY_ADDRESS			0x27

#define MY_ADDRESS_MASK		0xFE

int main(void)
{
	
	unsigned int Value;
	
	TWI_ADDRESS MyAddress;
	
	TWI_ADDRESS_MASK MyAddressMask;
	
	//
	// Init Tracing
	//
	
	InitTrace(F_CPU, ERROR_LEVEL | INFO_LEVEL);
	
	SendTrace(INFO_LEVEL, "TWI Slave Test\r\n");
	
	SendTrace(INFO_LEVEL, "---------------\r\n");
	
	//
	// Initialize TWI in Slave Mode
	//
	
	MyAddress.RW_EnGenCall = 1;
	
	MyAddress.Address = MY_ADDRESS;
	
	MyAddressMask.AsUINT8 = MY_ADDRESS_MASK;
	
	InitTWI(TWI_SLAVE, &MyAddress, &MyAddressMask);
	
	
	
	while(1)
	{
		TwiReceive(&Value, sizeof(unsigned int), NULL, FALSE);
		
		SendTrace(INFO_LEVEL, "Data Received = 0x%x\r\n", Value);
		
		Value = Value + 1;
		
		TwiTransmit(&Value, sizeof(unsigned int), NULL, FALSE);
		
		SendTrace(INFO_LEVEL, "Data Transmitted = 0x%x\r\n", Value);
		
		
	}
}