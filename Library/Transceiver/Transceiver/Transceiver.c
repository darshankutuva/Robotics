/*
 *  Transceiver.c
 *
 *  Author: Shanthanand R Kutuva
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

#include "TransceiverInternal.h"

#include "Trace.h"

#include "ExtInt.h"

//
// Global Variables
//

bool gIsAutoAcknowledgeEnabled;

bool gTransceiverInitialized		= FALSE;

PTCInterruptCallback gMaxRTCallback	= NULL;

PVOID gMaxRTCallbackContext			= NULL;

PTCInterruptCallback gRxDRCallback	= NULL;

PVOID gRxDRCallbackContext			= NULL;

PTCInterruptCallback gTxDSCallback	= NULL;

PVOID gTxDSCallbackContext			= NULL;

unsigned char gNOPBuffer[MAX_PAYLOAD_LENGTH];

TRANSCEIVER_ADDR gMyAddress;

TRANSCEIVER_ADDR gDestAddress;

//
// Public Functions
//

bool
InitTransceiver (PTRANSCEIVER_INIT_PARAMS InitParams)
{	
	
	CONFIG ConfigReg;
	
	EN_AA EnAA;
	
	EN_RX_ADDR EnRxAddr;
	
	SETUP_RETR SetupRetr;
	
	RF_SETUP RfSetup;
	
	DYNPD DynpdReg;
	
	PAYLOAD_LENGTH Length;
	
	FEATURE FeatureReg;
	
	unsigned char IndexI;
	
	if (InitParams == NULL)
	{
		SendTrace(ERROR_LEVEL, "InitTransceiver - Incorrect Initialization Parameters\r\n");
		
		goto Exit;
	}
	
	if (gTransceiverInitialized)
	{
		SendTrace(ERROR_LEVEL, "InitTransceiver - Transceiver Already Initialized\r\n");
		
		goto Exit;
	}
	
	//
	// Initialize NOP Buffer. The is required for all Receive operations.
	//
	
	for (IndexI = 0; IndexI < MAX_PAYLOAD_LENGTH; IndexI++)
	{
		gNOPBuffer[IndexI] = NOP;
	}
	
	//
	// Initialize Data Direction Registers
	// Make CE (PC7) output.
	// 
	
	DDRC |= (1 << PORTC7);
	
	//
	// Enable Interrupts
	//
	
	if (InitParams->MaxRTCallback &&
		InitParams->RxDRCallback &&
		InitParams->TxDSCallback)
	{
		if (!RegisterExtInterrupt(EXTINT_INT2, 
								  FALLING_EDGE_TRIGGERED, 
								  &TCIntCallback, 
								  NULL, 
								  TRUE))
		{
			goto Exit;
		}		
	} 
	
	//
	// Wait for Transceiver to Settle
	//
	
	CE_LOW();
	
	_delay_ms(100);
	
	//
	// Initialize Global Variables
	//
	
	gIsAutoAcknowledgeEnabled = InitParams->IsAutoAcknowledgeEnabled;
	
	//
	// Initialize Callbacks
	//
	
	gMaxRTCallback			= InitParams->MaxRTCallback;
	
	gMaxRTCallbackContext	= InitParams->MaxRTCallbackContext;
	
	gRxDRCallback			= InitParams->RxDRCallback;
	
	gRxDRCallbackContext	= InitParams->RxDRCallbackContext;
	
	gTxDSCallback			= InitParams->TxDSCallback;
	
	gTxDSCallbackContext	= InitParams->TxDSCallbackContext;
	
	//
	// Initialize SPI Transfer
	//

	TCInitTransfer();
	
	//
	// Initialize values of Config Register
	//
	
	ConfigReg.AsUINT8	= 0;
	
	if (InitParams->Mode == RECEIVER)
	{	
		ConfigReg.PRIM_RX	= 1;
	}
	
	ConfigReg.PWR_UP	= 1;
	
	ConfigReg.EN_CRC	= 1;
	
	
	TCWriteReg(CONFIG_OFFSET, &ConfigReg.AsUINT8, 1);
	
	ConfigReg.AsUINT8 = 0;
	
	//
	// Initialize EN_AA Register
	//
		
	EnAA.AsUINT8 = 0;
	
	if (InitParams->IsAutoAcknowledgeEnabled)
	{
		EnAA.DATA_PIPE = 0x1 << DATA_PIPE_0;
	}
	
	TCWriteReg(EN_AA_OFFSET, &EnAA.AsUINT8, 1);
		
	//
	// Initialize EN_RXADDR Register
	//
		
	EnRxAddr.AsUINT8 = 0;
		
	EnRxAddr.DATA_PIPE = 0x1 << DATA_PIPE_0;

	TCWriteReg(EN_RX_ADDR_OFFSET, &EnRxAddr.AsUINT8, 1);
		
	//
	// Initialize SETUP_RETR
	//
		
	SetupRetr.ARD = InitParams->AutoRetryDelay;
		
	SetupRetr.ARC = InitParams->AutoRetryCount;
		
	TCWriteReg(SETUP_RETR_OFFSET, &SetupRetr.AsUINT8, 1);
	
	//
	// Initialize RF Channel
	//
	
	TCWriteReg(RF_CH_OFFSET, &InitParams->RFChannel.AsUINT8, 1);
	
	//
	// Initialize RF Setup
	//
	
	RfSetup.AsUINT8 = 0;
	
	RfSetup.RF_DR_LOW = 0;
	
	RfSetup.RF_DR_HIGH = 1;
	
	RfSetup.RF_PWR = ZerodBm;
	
	TCWriteReg(RF_SETUP_OFFSET, &RfSetup.AsUINT8, 1);
	
	//
	// Initialize Address
	//
	
	gMyAddress.AsULongLong = InitParams->MyAddress.AsULongLong;
	
	gDestAddress.AsULongLong = InitParams->MyAddress.AsULongLong;
	
	if (InitParams->IsAutoAcknowledgeEnabled)
	{	
		if (InitParams->Mode == RECEIVER)
		{
			//
			// Initialize RX_ADDR_P0
			//
			
			TCWriteReg(RX_ADDR_P0_OFFSET, &InitParams->MyAddress.AsULongLong, TRANSCEIVER_ADDR_LENGTH);
			
		}
		else
		{
			//
			// Initialize RX_ADDR_P0
			//
			
			TCWriteReg(RX_ADDR_P0_OFFSET, &InitParams->DestAddress.AsULongLong, TRANSCEIVER_ADDR_LENGTH);
			
			//
			// Initialize TX_ADDR
			//
			
			TCWriteReg(TX_ADDR_OFFSET, &InitParams->DestAddress.AsULongLong, TRANSCEIVER_ADDR_LENGTH);
		}
	}
	else
	{
		//
		// Initialize RX_ADDR_P0
		//
		
		TCWriteReg(RX_ADDR_P0_OFFSET, &InitParams->MyAddress.AsULongLong, TRANSCEIVER_ADDR_LENGTH);
		
		//
		// Initialize TX_ADDR
		//
		
		TCWriteReg(TX_ADDR_OFFSET, &InitParams->DestAddress.AsULongLong, TRANSCEIVER_ADDR_LENGTH);
	}
	
	//
	// Initialize Payload Length to MAX PAYLOAD LENGTH if Auto Acknowledgment is not enabled
	//
	
	if (!InitParams->IsAutoAcknowledgeEnabled)
	{
		Length.PayloadLength = MAX_PAYLOAD_LENGTH;
		
		TCWriteReg(RX_PW_P0_OFFSET, &Length, 1);
	}
	//
	// Initialize Dynamic Payload Length	
	//
	
	if (InitParams->IsAutoAcknowledgeEnabled)
	{
		DynpdReg.AsUINT8 = 0;
	
		DynpdReg.DPL_P0 = 0x1 << DATA_PIPE_0;
	
		TCWriteReg(DYNPD_OFFSET, &DynpdReg.AsUINT8, 1);
	}
	
	//
	// Initialize Feature
	//
	
	FeatureReg.AsUINT8 = 0;
		
	FeatureReg.EN_ACK_PAY = 1;
	
	if (InitParams->IsAutoAcknowledgeEnabled)
	{
		FeatureReg.EN_DPL = 1;
	}
	
	TCWriteReg(FEATURE_OFFSET, &FeatureReg.AsUINT8, 1);
	
	//
	// Flush RX and TX FIFO
	//
	
	TCFlushFIFO(RX_FIFO);
	
	TCFlushFIFO(TX_FIFO);
	
	//
	// If Receiver start monitoring for packets.
	//
	
	if (InitParams->Mode == RECEIVER)
	{
		CE_HIGH();
		
		_delay_us(130);
	}
	
	gTransceiverInitialized = TRUE;
	
Exit:

	return gTransceiverInitialized;
}

unsigned char
TCGetLostPacketCount()
{
	unsigned char Value = 0;
	
	OBSERVE_TX ObserveTx;
	
	//
	// Check if Transceiver is initialized.
	//
	
	if (!gTransceiverInitialized)
	{
		SendTrace(ERROR_LEVEL, "GetLostPacketCount - Transceiver Not Initialized\r\n");
		
		return 0;
	}
	
	//
	// Read Observe TX Register
	//
	
	if (TCReadReg(OBSERVE_TX_OFFSET, &ObserveTx.AsUINT8, 1))
	{
		Value = ObserveTx.PLOS_CNT >> 4;;
	}
	else
	{
		SendTrace(ERROR_LEVEL, "GetLostPacketCount - Read ObserveTx Register Failed\r\n");
	}
	
	return Value;
}

unsigned char
TCGetRetransmitPacketCount()
{
	unsigned char Value = 0;
		
	OBSERVE_TX ObserveTx;
	
	//
	// Check if Transceiver is initialized;
	//
	
	if (!gTransceiverInitialized)
	{
		SendTrace(ERROR_LEVEL, "GetRetransmitPacketCount - Transceiver Not Initialized\r\n");
		
		return 0;
	}
	
	//
	// Read Observe TX Register
	//
		
	if (TCReadReg(OBSERVE_TX_OFFSET, &ObserveTx.AsUINT8, 1))
	{
		Value = ObserveTx.ARC_CNT;
	}
	else
	{
		SendTrace(ERROR_LEVEL, "GetRetransmitPacketCount - Read ObserveTx Register Failed\r\n");
	}
	
	return Value;	
}

void
TCGetReceivedPayloadLength(PPAYLOAD_LENGTH Length)
{
	unsigned char CommandStatus;
	
	bool Status;
	
	//
	// Check if Transceiver is initialized;
	//
	
	if (!gTransceiverInitialized)
	{
		SendTrace(ERROR_LEVEL, "GetReceivedPayloadLength - Transceiver Not Initialized\r\n");
		
		return;
	}
	
	Length->AsUINT8 = 0;
	
	if (gIsAutoAcknowledgeEnabled)
	{
		//
		// If Auto Acknowledgment is enabled then Dynamic Payload length is enabled.
		// So read payload length.
		//
		
		CommandStatus = R_RX_PL_WID;
		
		Status = TCReceive(&CommandStatus, &Length->AsUINT8, 1);
		
		if (!Status || Length->AsUINT8 > MAX_PAYLOAD_LENGTH)
		{
			Length->AsUINT8 = 0;
			
			TCFlushFIFO(RX_FIFO);
		
			SendTrace(ERROR_LEVEL, "GetReceivedPayloadLength - Read Rx Payload Length Register Failed\r\n");
		}
	}
	else
	{
		//
		// If Auto Acknowledgment is not enabled then Payload length is set to MAX_PAYLOAD_LENGTH.
		//
		
		Length->PayloadLength = MAX_PAYLOAD_LENGTH;
	}
}

bool
TCIsReceivePayloadAvailable()
{
	bool Status = FALSE;
	
	STATUS_REG StatusReg;
	
	TCReadReg(STATUS_OFFSET, &StatusReg.AsUINT8, 1);
	
	if (StatusReg.RX_DR)
	{
		//
		// Clear StatusReg RX_DR
		//
		
		StatusReg.AsUINT8 = 0;
		
		StatusReg.RX_DR = 1;
		
		TCWriteReg(STATUS_OFFSET, &StatusReg.AsUINT8, 1);
		
		SendTrace(INFO_LEVEL, "Data Received\r\n");
		
		Status = TRUE;
	}
	
	return Status;
}

bool
TCReceivePayload(PVOID Payload, PPAYLOAD_LENGTH Length)
{
	bool Status = TRUE;
	
	PAYLOAD_LENGTH PayloadLength;
	
	unsigned char CommandStatus;
	
	//
	// Check if Transceiver is initialized;
	//
	
	if (!gTransceiverInitialized)
	{
		SendTrace(ERROR_LEVEL, "ReceivePayload - Transceiver Not Initialized\r\n");
		
		Status = FALSE;
		
		goto Exit;
	}
	
	PayloadLength.AsUINT8 = 0;
	
	//
	// Check if Length specified in the params matches the actual received payload length
	//
	
	TCGetReceivedPayloadLength(&PayloadLength);
	
	if (Length->PayloadLength > PayloadLength.PayloadLength)
	{
		SendTrace(INFO_LEVEL, "ReceivePayload - Rx Payload Length Not Matching Buffer Length\r\n");
		
		Length->PayloadLength = PayloadLength.PayloadLength;
	}
	
	//
	// Initialize Command
	//
	
	CommandStatus = R_RX_PAYLOAD;
	
	Status = TCReceive(&CommandStatus, Payload, Length->AsUINT8);
	
Exit:
	
	return Status;

}

bool
TCTransmitPayload(PVOID Payload, PPAYLOAD_LENGTH Length)
{

	unsigned char CommandStatus;
	
	STATUS_REG StatusReg;
	
	bool Status;
	
	//
	// Check if Transceiver is initialized;
	//
	
	if (!gTransceiverInitialized)
	{
		SendTrace(ERROR_LEVEL, "TransmitPayload - Transceiver Not Initialized\r\n");
		
		Status = FALSE;
		
		goto Exit;
	}
	
	//
	// Initialize Command Register
	//
	
	CommandStatus = W_TX_PAYLOAD;
	
	Status = TCTransmit(&CommandStatus, Payload, Length->AsUINT8);
	
	//
	// Now kick of the transmission.
	//
	
	CE_HIGH();
	
	//_delay_us(130);
	
	//CE_LOW();
	
	if (gTxDSCallback == NULL)
	{
		//
		// Data Sent Interrupt Callback has not been registered by the application
		// So wait till data is transmitted.
		//
		
		while(1)
		{
			TCReadReg(STATUS_OFFSET, &StatusReg.AsUINT8, 1);
			
			if (StatusReg.TX_DS)
			{
				//
				// Clear the TX_DS bit
				//
				
				StatusReg.AsUINT8 = 0;
				
				StatusReg.TX_DS = 1;
				
				TCWriteReg(STATUS_OFFSET, &StatusReg.AsUINT8, 1);
					
				SendTrace(INFO_LEVEL, "TransmitPayload - Data Transmitted\r\n");
			
				break;
			}
			else if (StatusReg.MAX_RT)
			{	
				//
				// Clear the MAX_RT bit
				//
				
				StatusReg.AsUINT8 = 0;
				
				StatusReg.MAX_RT = 1;
				
				TCWriteReg(STATUS_OFFSET, &StatusReg.AsUINT8, 1);
				
				SendTrace(ERROR_LEVEL,"TransmitPayload - Max Re-transmit Reached\r\n");
				
				Status = FALSE;
				
				break;
			}
		}
	}
	
	CE_LOW();
	
Exit:
	
	return Status;

}

bool
TCTransmitAckPayload(PVOID Payload, PPAYLOAD_LENGTH Length)
{	
	unsigned char CommandStatus;
	
	bool Status;
	
	//
	// Check if Transceiver is initialized;
	//
	
	if (!gTransceiverInitialized)
	{
		SendTrace(ERROR_LEVEL, "TransmitAckPayload - Transceiver Not Initialized\r\n");
		
		Status = FALSE;
		
		goto Exit;
	}
	
	if (!gIsAutoAcknowledgeEnabled)
	{
		SendTrace(ERROR_LEVEL, "TransmitAckPayload - Auto Acknowledgment Not Enabled\r\n");
		
		Status = FALSE;
		
		goto Exit;
	}
	
	//
	// Initialize Command Register
	//
	
	CommandStatus = W_ACK_PAYLOAD;
	
	Status = TCTransmit(&CommandStatus, Payload, Length->AsUINT8);
	
Exit:
	
	return Status;
}

//
// Private Functions
//

//
// Interrupt Callback
//

void
TCIntCallback(PVOID CallbackContext)
{
		
	STATUS_REG StatusReg;
	
	STATUS_REG ClearInt;
	
	//
	// Read STATUS Register to find the cause of interrupt
	//
	
	TCReadReg(STATUS_OFFSET, &StatusReg.AsUINT8, 1);
	
	SendTrace(INFO_LEVEL, "ISR - Status Reg = 0x%x\r\n", StatusReg.AsUINT8);
	
	ClearInt.AsUINT8 = 0;
	
	if (StatusReg.MAX_RT && gMaxRTCallback != NULL)
	{
		gMaxRTCallback(gMaxRTCallbackContext);
		
		ClearInt.MAX_RT = 1;
	}
	
	if (StatusReg.RX_DR && gRxDRCallback != NULL)
	{
		gRxDRCallback(gRxDRCallbackContext);
		
		ClearInt.RX_DR = 1;
	}
	
	if (StatusReg.TX_DS && gTxDSCallback != NULL)
	{
		gTxDSCallback(gTxDSCallbackContext);
		
		ClearInt.TX_DS = 1;
	}
	
	//
	// Clear the interrupts
	//
	
	TCWriteReg(STATUS_OFFSET, &ClearInt.AsUINT8, 1);
} 

void
TCFlushFIFO(FIFO_TYPE FIFOType)
{
	unsigned char CommandStatus;
	
	if(FIFOType == RX_FIFO)
	{
		CommandStatus = FLUSH_RX;
	}
	else
	{
		CommandStatus = FLUSH_TX;
	}
	
	TCBeginTransfer();
	
	TCTransferSync(&CommandStatus, &CommandStatus, 1);

	TCEndTransfer();
}

bool
TCWriteReg(unsigned char RegOffset, PVOID Value, unsigned char Length)
{	
	unsigned char CommandStatus;
	
	unsigned char ReadValue = 0;
	
	bool Status;
	
	//
	// Initialize Command Register
	//
	
	CommandStatus = W_REGISTER | RegOffset;
	
	Status = TCTransmit(&CommandStatus, Value, Length);
	
	TCReadReg(RegOffset, &ReadValue, 1);
	
	SendTrace(INFO_LEVEL, "Reg Offset = 0x%x Value = 0x%x\r\n", RegOffset, ReadValue);
	
	return Status;
}

bool
TCReadReg(unsigned char RegOffset, PVOID Value, unsigned char Length)
{
			
	unsigned char CommandStatus;
	
	bool Status;
	
	//
	// Initialize Command Register
	//
		
	CommandStatus = R_REGISTER | RegOffset;
	
	Status = TCReceive(&CommandStatus, Value, Length);
	
	return Status;
	
}

bool
TCTransmit(unsigned char *CommandStatus, PVOID Value, unsigned char Length)
{
	bool Status;
	
	bool TransmitPadding = FALSE;
		
	unsigned char DummyBuffer[MAX_PAYLOAD_LENGTH];
	
	unsigned char IndexI;	
	
	//
	// Check if we need to transmit padding
	//
	
	if ((*CommandStatus == W_TX_PAYLOAD) &&
		(!gIsAutoAcknowledgeEnabled) &&
		(Length < MAX_PAYLOAD_LENGTH))
	{	
		TransmitPadding = TRUE;
	}
	
	if (TransmitPadding)
	{
		//
		// Initialize Padding in Dummy Buffer
		//
		
		for (IndexI = Length; IndexI < MAX_PAYLOAD_LENGTH; IndexI++)
		{
			DummyBuffer[IndexI] = 0x0;
		}
	}	

	TCBeginTransfer();
	
	//
	// Send the command and get the status back.
	//
		
	Status = TCTransferSync(CommandStatus, CommandStatus, 1);
		
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Transmit - Transfer Failed\r\n");
			
		goto Exit;
	}
	
	//
	// Now transmit the payload.
	//
		
	Status = TCTransferSync(Value, &DummyBuffer[0], Length);
		
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Transmit - Transfer Failed\r\n");
			
		goto Exit;
	}
	
	if (TransmitPadding)
	{
		//
		// Transmit the padding
		//
		
		Status = TCTransferSync(&DummyBuffer[Length], &DummyBuffer[Length], MAX_PAYLOAD_LENGTH - Length);
		
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Transmit - Transfer Failed\r\n");
			
			goto Exit;
		}
	}
	
Exit:

	TCEndTransfer();
		
	return Status;
}

bool
TCReceive(unsigned char *CommandStatus, PVOID Value, unsigned char Length)
{
	bool Status;
	
	TCBeginTransfer();
	
	//
	// Transmit the command and get the status
	//
	
	Status = TCTransferSync(CommandStatus, CommandStatus, 1);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Receive - Transfer Failed\r\n");
		
		goto Exit;
	}
	
	//
	// Now receive the payload.
	//
	
	Status = TCTransferSync(&gNOPBuffer[0], Value, Length);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Receive - Transfer Failed\r\n");
		
		goto Exit;
	}
	
Exit:
	
	TCEndTransfer();
	
	return Status;
		
}
