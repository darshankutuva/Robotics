/*
 * TransceiverMasterTest.c
 *
 * Author: Shanthanand R Kutuva
 */ 


#include <avr/io.h>

#include "TransceiverMasterTest.h"

#include "Trace.h"

int main(void)
{
	
	unsigned char Value = 1;
	
	TRANSCEIVER_INIT_PARAMS InitParams;

	PAYLOAD_LENGTH Length;

	//bool IsDataSent;
	
	//
	// Initialize Trace
	//
	
	InitTrace(F_CPU, INFO_LEVEL | ERROR_LEVEL);
	
	SendTrace(INFO_LEVEL, "Transceiver Master Test\r\n");
	
	SendTrace(INFO_LEVEL, "-----------------------\r\n");
	
	//
	// Initialize Mode
	//
	
	InitParams.Mode = TRANSMITTER;
	
	//
	// Initialize Auto Acknowledgment
	//
	
	InitParams.IsAutoAcknowledgeEnabled = TRUE;
	
	//
	// Initialize MY Address
	//
	
	InitParams.MyAddress.AsULongLong = 0x0;
	
	InitParams.MyAddress.Address = 0xa0b0c0d0e0;
	
	//
	// Initialize Destination Address
	//
	
	InitParams.DestAddress.AsULongLong = 0x0;
	
	InitParams.DestAddress.Address = 0x7878787878;
	
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
	
	//IsDataSent = FALSE;
	
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
		
		Length.PayloadLength = sizeof(unsigned char);
		
		TCTransmitPayload((PVOID) &Value, &Length);
		
		Value++;
    }
}

void
TxDSCallback(PVOID CallbackContext)
{
	bool *IsDataSent = (bool *) CallbackContext;
	
	SendTrace(INFO_LEVEL,"Data Transmitted\r\n");
	
	*IsDataSent = TRUE; 
}