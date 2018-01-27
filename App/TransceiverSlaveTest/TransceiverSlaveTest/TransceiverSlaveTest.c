/*
 * TransceiverSlaveTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 

#include <avr/io.h>

#include "TransceiverSlaveTest.h"

#include "Trace.h"

int main(void)
{
	
	unsigned char Value[32];
	
	TRANSCEIVER_INIT_PARAMS InitParams;

	PAYLOAD_LENGTH Length;

	//bool IsDataReceived;
	
	//
	// Initalize Trace
	//
	
	InitTrace(F_CPU, INFO_LEVEL | ERROR_LEVEL);
	
	SendTrace(INFO_LEVEL, "Transceiver Slave Test\r\n");
	
	SendTrace(INFO_LEVEL, "-----------------------\r\n");
	
	//
	// Initialize Mode
	//
	
	InitParams.Mode = RECEIVER;
	
	//
	// Initialize Auto Acknowledgment
	//
	
	InitParams.IsAutoAcknowledgeEnabled = TRUE;
	
	//
	// Initialize MY Address
	//
	
	InitParams.MyAddress.AsULongLong = 0x0;
	
	InitParams.MyAddress.Address = 0x7878787878;
	
	//
	// Initialize Destination Address
	//
	
	InitParams.DestAddress.AsULongLong = 0x0;
	
	InitParams.DestAddress.Address = 0xa0b0c0d0e0;
	
	//
	// Initialize Retry parameters
	//
	
	InitParams.AutoRetryCount = ARC_15;
	
	InitParams.AutoRetryDelay = ARD_4000_MS;
	
	//
	// Initialize RF Channel
	//
	
	InitParams.RFChannel.AsUINT8 = 0;
	
	InitParams.RFChannel.RFCH = 110;
	
	//
	// Register Interrupt Callbacks
	//
	
	InitParams.MaxRTCallback = NULL;
	
	InitParams.RxDRCallback = NULL;
	
	InitParams.TxDSCallback = NULL;
	
	//
	// Initialize call back context
	//
	
	//IsDataReceived = FALSE;
	
	InitParams.MaxRTCallbackContext = NULL;
	
	InitParams.TxDSCallbackContext	= NULL;
	
	InitParams.RxDRCallbackContext	= NULL;
	
	//
	// Call initialization routine
	//
	
	InitTransceiver(&InitParams);
	
	while(1)
	{
		Length.AsUINT8 = 0;
		
		while(!TCIsReceivePayloadAvailable());
		
		TCGetReceivedPayloadLength(&Length);
		
		SendTrace(INFO_LEVEL, "Length = %d\r\n", Length.AsUINT8);
			
		TCReceivePayload((PVOID) &Value[0], &Length);
			
		SendTrace(INFO_LEVEL, "Value = 0x%x\r\n", Value[0]);
	}
}

void
RxDRCallback(PVOID CallbackContext)
{
	bool *IsDataReceived = (bool *) CallbackContext;

	SendTrace(INFO_LEVEL,"Data Received\r\n");
	
	*IsDataReceived = TRUE;
}