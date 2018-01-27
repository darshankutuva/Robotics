/*
 * XY_MK_5V_XY_FST.c
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

#include "XY-MK-5V-XY-FST.h"

#include "UART.h"

#include "Trace.h"

//
// Constant Definition
//

#define RECEIVE_TIMEOUT			10

//
// Global Variable Definition
//

unsigned char gMyAddress;

unsigned char gDestAddress;

unsigned long  gMessageId;

bool gXymk5vXyfstInitialized = FALSE;

bool gXymk5vXyfstAutoRetransmit;



bool 
InitXymk5vXyfst(unsigned char MyAddress, unsigned char DestAddress, bool EnableAutoReTransmit)
{
    bool Status;
	
	Status = InitUARTSync(UART_ONE, BR_2400, MULTI_PROCESSOR_COMMUNICATION_MODE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Failed to initialized UART 1\r\n");
		
		goto Exit;
	}

	gMyAddress = MyAddress;
	
	gDestAddress = gDestAddress;

	gXymk5vXyfstAutoRetransmit = EnableAutoReTransmit;
	
	gXymk5vXyfstInitialized = TRUE;

Exit:

    return Status;
}

bool
Xymk5vXyfstTransmit(PVOID Buffer, unsigned long Length, MESSAGE_TYPE MessageType)
{
	bool Status = FALSE;
	
	unsigned long AckMessage = 0;
	
	unsigned long AckMessageLength;
	
	bool IsAckReceived = 0;
	
	//
	// Check if Xymk5vXyfst is initialized.
	//
	
	if (!gXymk5vXyfstInitialized)
	{
		SendTrace(ERROR_LEVEL, "Xymk5vXyfst not Initialized\r\n");
		
		goto Exit;
	}
	
	//
	// Now disable our receiver UART
	//
	
	Status = UARTEnableDisableReceiver(UART_ONE, DISABLE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Unable to disable receiver\r\n");
		
		goto Exit;
	}
	
	if (MessageType == ACK_MESSAGE)
	{
		SendTrace(INFO_LEVEL, "Transmitting Ack Message\r\n");
		
		//
		// First transmit the destination address
		//
		
		Status = UARTTransmitAddress(UART_ONE, gDestAddress);
		
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to transmit Dest Address\r\n");
			
			Status = FALSE;
			
			goto Exit;
		}
		
		_delay_ms(1);
		
		//
		// Now transmit the buffer
		//
		
		Status = UARTTransmitSync(UART_ONE, Buffer, Length);
		
		if (!Status)
		{
			SendTrace(ERROR_LEVEL, "Failed to transmit Buffer\r\n");
			
			Status = FALSE;
			
			goto Exit;
		}
	}
	else
	{
	
		do
		{
			SendTrace(INFO_LEVEL, "Transmitting General Message\r\n");
			
			//
			// First transmit the destination address
			//
	
			Status = UARTTransmitAddress(UART_ONE, gDestAddress);
	
			if (!Status)
			{
				SendTrace(ERROR_LEVEL, "Failed to transmit Dest Address\r\n");
				
				Status = FALSE;
		
				goto Exit;
			}
			
			_delay_ms(1);
	
			//
			// Now transmit the Message ID
			//
	
			Status = UARTTransmitSync(UART_ONE, &gMessageId, sizeof(unsigned long));
	
			if (!Status)
			{
				SendTrace(ERROR_LEVEL, "Failed to transmit Message ID\r\n");
				
				Status = FALSE;
		
				goto Exit;
			}

			//
			// Now transmit the length of the message
			//
	
			Status = UARTTransmitSync(UART_ONE, &Length, sizeof(unsigned long));
	
			if (!Status)
			{
				SendTrace(ERROR_LEVEL, "Failed to transmit Length\r\n");
				
				Status = FALSE;
		
				goto Exit;
			}
	
			//
			// Now transmit the buffer
			//
	
			Status = UARTTransmitSync(UART_ONE, Buffer, Length);
	
			if (!Status)
			{
				SendTrace(ERROR_LEVEL, "Failed to transmit Buffer\r\n");
				
				Status = FALSE;
		
				goto Exit;
			}
		
			//
			// Now receive the acknowledgment
			//
		
			AckMessageLength = sizeof(unsigned long);
		
			Status = Xymk5vXyfstReceive(&AckMessage, &AckMessageLength, ACK_MESSAGE);
		
			if (!Status)
			{
				SendTrace (INFO_LEVEL, "Failed to receive Ack Message\r\n");
			}
			else if ((AckMessage == (gMessageId + 1)) || !gXymk5vXyfstAutoRetransmit)
			{
				IsAckReceived = 1;
			}
		
		} while(IsAckReceived == 0);
		
		gMessageId++;
	}
	
Exit:

	//
	// Now enable our receiver after transmit is over
	//
	
	UARTEnableDisableReceiver(UART_ONE, ENABLE);

	return Status;	
}

bool
Xymk5vXyfstReceive(PVOID Buffer, unsigned long *Length, MESSAGE_TYPE MessageType)
{

	bool Status = FALSE;

	unsigned long Len;
	
	unsigned char DestAddress;
	
	unsigned long MessageId;
	
	unsigned long MessageLength;
	
	//
	// Enable Receiver UART
	//
	
	Status = UARTEnableDisableReceiver(UART_ONE, ENABLE);
	
	if (!Status)
	{
		SendTrace(ERROR_LEVEL, "Unable to enable receiver\r\n");
		
		goto Exit;
	}
		
	if (MessageType == GENERAL_MESSAGE)
	{	
		SendTrace(INFO_LEVEL, "General Message - Waiting for Address\r\n");
		
		do
		{
			//
			// First receive the destination address and match it with
			// My address
			//
			
			Len = UARTReceiveSync(UART_ONE, &DestAddress, sizeof(unsigned char));
			
			if (Len != sizeof(unsigned char))
			{
				SendTrace(ERROR_LEVEL, "Invalid Destination Address.\r\n");
				
				Status = FALSE;
				
				goto Exit;
			}
			
		} while(DestAddress != gMyAddress);
		
		SendTrace(INFO_LEVEL, "General Message - Receiving New Message\r\n");
		
		//
		// Disable MPCM Mode
		//
		
		UARTEnableDisableMPCM(UART_ONE, DISABLE);	
		//
		// Get Message ID
		//
	
		Len = UARTReceiveASync(UART_ONE, &MessageId, sizeof(unsigned long), RECEIVE_TIMEOUT);
	
		if (Len != sizeof(unsigned long))
		{
			SendTrace(ERROR_LEVEL, "Failed to receive Message Id.\r\n");
			
			Status = FALSE;
		
			goto Exit;
		}
		
		SendTrace(INFO_LEVEL, "Message Id = %d\r\n", MessageId);
		
		//
		// Get Message Length
		//
		
		Len = UARTReceiveASync(UART_ONE, &MessageLength, sizeof(unsigned long), RECEIVE_TIMEOUT);
		
		if (Len != sizeof(unsigned long))
		{
			SendTrace(ERROR_LEVEL, "Failed to receive Message Length.\r\n");
			
			Status = FALSE;
			
			goto Exit;
		}
		
		//
		// Get Actual Message
		//
		
		//
		// Verify we have enough buffer to store the received message.
		//
			
		if (*Length < MessageLength)
		{
			SendTrace(ERROR_LEVEL, "Insufficient Receive Buffer - Buffer Length = %d, Message Length = %d\r\n", *Length, MessageLength);
				
			*Length = 0;
			
			Status = FALSE;
				
			goto Exit;
		}
			
		//
		// Receive the message
		//
			
		*Length = UARTReceiveASync(UART_ONE, Buffer, MessageLength, RECEIVE_TIMEOUT);
			
		if (*Length != MessageLength)
		{
			SendTrace(ERROR_LEVEL, "Failed to receive entire message, Expected Message Length = %d, Received Message Length = %d.\r\n", MessageLength, *Length);
			
			Status = FALSE;
				
			goto Exit;
		}
			
		//
		// We have received the entire message without error, so send a Ack Message
		//
			
		MessageId++;
				
		Xymk5vXyfstTransmit(&MessageId, sizeof(unsigned long), ACK_MESSAGE);
				
	}
	else
	{
		SendTrace(INFO_LEVEL, "Ack Message - Waiting for Address\r\n");
		
		do
		{
			//
			// First receive the destination address and match it with
			// My address
			//
			
			Len = UARTReceiveASync(UART_ONE, &DestAddress, sizeof(unsigned char), RECEIVE_TIMEOUT);
			
			if (Len != sizeof(unsigned char))
			{
				SendTrace(ERROR_LEVEL, "Invalid Destination Address.\r\n");
				
				Status = FALSE;
				
				goto Exit;
			}
			
		} while(DestAddress != gMyAddress);
		
		SendTrace(INFO_LEVEL, "Ack Message - Receiving New Message\r\n");
		
		//
		// Disable MPCM Mode
		//
		
		UARTEnableDisableMPCM(UART_ONE, DISABLE);
		
		//
		// Verify we have enough buffer to store the received message.
		//
			
		if (*Length < sizeof(unsigned long))
		{
			SendTrace(ERROR_LEVEL, "Insufficient Receive Buffer \r\n");
				
			*Length = 0;
			
			Status = FALSE;
				
			goto Exit;
		}
			
		//
		// Receive the message
		//
			
		*Length = UARTReceiveASync(UART_ONE, Buffer, sizeof(unsigned long), RECEIVE_TIMEOUT);
			
		if (*Length != sizeof(unsigned long))
		{
			SendTrace(ERROR_LEVEL, "Failed to receive entire message, Received Message Length = %d.\r\n", *Length);
			
			Status = FALSE;
				
			goto Exit;
		}
	}

Exit:

	UARTEnableDisableMPCM(UART_ONE, ENABLE);
	
	return Status;
}
