/*
 *  TWI.c
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
 
#include "TWI.h"

#include "TWIInternal.h"

#include "Trace.h"

bool gIsTWIInitialized = FALSE;

bool gIsRepeatedStart = FALSE;

TWI_MODE gMode;

//
// Public Functions
//

bool
InitTWI(TWI_MODE Mode, PTWI_ADDRESS Address, PTWI_ADDRESS_MASK AddressMask)
{	
	bool Status = TRUE;
	
	if (Mode == TWI_SLAVE)
	{
		if (Address != NULL && AddressMask != NULL)
		{
			//
			// Set Slave Address and Mask
			//
				
			TWAR = Address->AsUINT8;
				
			TWAMR = AddressMask->AsUINT8;
			
			//
			// Set Data and Clock to IN
			//
			
			DDRC &= ~((0x1 << PORTC0) | (0x1 << PORTC1));
			
			gMode = TWI_SLAVE;
		}
		else
		{
			SendTrace(ERROR_LEVEL, "InitTWI - Invalid Parameters\r\n");
			
			Status = FALSE;
			
			goto Exit;
		}
	}
	else
	{
		gMode = TWI_MASTER;
		
		//
		// Set Data and Clock to Out
		//
		
		DDRC |= ((0x1 << PORTC0) | (0x1 << PORTC1));
	}
	
	//
	// Set TWI Bit Rate Register to 8 so that
	// SCL is set to 100 KHz.
	//

	TWBR = BIT_RATE_VALUE;
	
	//
	// Set Pre-scalar value to 1
	//

	TWSR = 0;
	
	//
	// Set TWI Control Register
	//
	
	TWCR = (0x1 << TWEN) | (0x1 << TWEA);
	
	//
	// Wait for 100 ms for bus to settle down
	//
	
	_delay_ms(100);
	
	gIsRepeatedStart = FALSE;
	
	gIsTWIInitialized = TRUE;

Exit:

	return Status;
}

bool 
TwiTransmit(PVOID Buffer, 
			unsigned long Length,
			PTWI_ADDRESS Address,
			bool SendStop)
{
	bool Status = TRUE;
	
	if (!gIsTWIInitialized || (Buffer == NULL))
	{
		Status = FALSE;
		
		goto Exit;
	}
	
	if (gMode == TWI_MASTER)
	{
		if (Address == NULL)
		{
			SendTrace(ERROR_LEVEL, "TwiTransmit - Invalid Parameters\r\n");
			
			Status = FALSE;
		
			goto Exit;
		}
		else
		{
			Status = TwiMasterTransmit(Buffer, Length, Address, SendStop);	
		}
	}
	else
	{
		WAIT_FOR_OP_COMPLETION();
		
		if (ProcessTwiStatus(STOP_REPEATED_START_RECEIVED))
		{
			WAIT_FOR_OP_COMPLETION();	
		}
		
		if (!ProcessTwiStatus(SLA_R_RECEIVED_ACK))
		{
			SendTrace(ERROR_LEVEL, "TwiTransmit - Slave Read Not Requested by Master - 0x%x\r\n", TWSR);
			
			Status = FALSE;
		
			goto Exit;
		}
		else
		{	
			DATA_OUT();
		
			Status = Transmit(Buffer, Length);
		
			DATA_IN();
		}
	}
	
Exit:

	return Status;
}

bool
TwiReceive(PVOID Buffer,
		   unsigned long Length,
		   PTWI_ADDRESS Address,
		   bool SendStop)
{
	bool Status = TRUE;
	
	if (!gIsTWIInitialized || (Buffer == NULL))
	{
		Status = FALSE;
		
		goto Exit;
	}
	
	if (gMode == TWI_MASTER)
	{
		if (Address == NULL)
		{
			SendTrace(ERROR_LEVEL, "TwiReceive - Invalid Parameters\r\n");
			
			Status = FALSE;
			
			goto Exit;
		}
		else
		{
			DATA_IN();
			
			Status = TwiMasterReceive(Buffer, Length, Address, SendStop);
			
			DATA_OUT();
		}
	}
	else
	{
		WAIT_FOR_OP_COMPLETION();
		
		if (!ProcessTwiStatus(SLA_W_RECEIVED_ACK))
		{
			SendTrace(ERROR_LEVEL, "TwiReceive - Slave Write Not Requested by Master - Status 0x%x\r\n", TWSR);
			
			Status = FALSE;
			
			goto Exit;
		}
		else
		{	
			Status = Receive(Buffer, Length);
		}
	}
	
Exit:

	return Status;		   
}


//
// Private Functions
//

bool
TwiMasterTransmit(PVOID Buffer, 
				  unsigned long Length, 
				  PTWI_ADDRESS Address,
				  bool SendStop)
{
	bool Status = TRUE;
	
	if (!SendStartCondition())
	{
		SendTrace(ERROR_LEVEL, "TwiMasterTransmit - Start Condition Transmission Failed - Status = 0x%x\r\n", (TWSR & TWI_STATUS_MASK));
		
		Status = FALSE;
			
		goto Exit;
	}
	
	gIsRepeatedStart = FALSE;
	
	
	//
	// Set Address->ReadWrite = 0 indicating a write operation.
	// Then transmit SLA+W
	//
	
	Address->RW_EnGenCall = 0;
	
	TWDR = Address->AsUINT8;
		
	WAIT_FOR_OP_COMPLETION();
		
	if (!ProcessTwiStatus(SLA_W_TRANSMITTED_ACK))
	{
		SendTrace(ERROR_LEVEL, "TwiMasterTransmit - Slave Address and Write Flag Transmission Failed - Status = 0x%x\r\n", (TWSR & TWI_STATUS_MASK));
		
		Status = FALSE;
			
		goto Exit;
	}
	
	Status = Transmit(Buffer, Length);
	
	//
	// Send stop condition
	//
	
	if (SendStop)
	{
		SendStopCondition();
	}
	else
	{
		gIsRepeatedStart = TRUE;
	}
	
Exit:

	return Status;
}

bool
TwiMasterReceive(PVOID Buffer, 
				 unsigned long Length, 
				 PTWI_ADDRESS Address,
				 bool SendStop)
{
	bool Status = TRUE;
	
	if (!SendStartCondition())
	{
		SendTrace(ERROR_LEVEL, "TwiMasterReceive - Start Condition Transmission Failed - Status = 0x%x\r\n", (TWSR & TWI_STATUS_MASK));
		
		Status = FALSE;
		
		goto Exit;
	}
	
	gIsRepeatedStart = FALSE;
	
	//
	// Set Address->ReadWrite = 1 indicating a read operation.
	// Then transmit SLA+R
	//
	
	Address->RW_EnGenCall = 1;
	
	TWDR = Address->AsUINT8;
	
	WAIT_FOR_OP_COMPLETION();
	
	if (!ProcessTwiStatus(SLA_R_TRANSMITTED_ACK))
	{
		SendTrace(ERROR_LEVEL, "TwiMasterReceive - Slave Address and Read Flag Transmission Failed - Status = 0x%x\r\n", (TWSR & TWI_STATUS_MASK));
		
		Status = FALSE;
		
		goto Exit;
	}
	
	Status = Receive(Buffer, Length);
	
	//
	// Send stop condition
	//
	
	if (SendStop)
	{
		SendStopCondition();
	}
	else
	{
		gIsRepeatedStart = TRUE;
	}
	
Exit:

	return Status;
}

bool
SendStartCondition()
{
	bool Status = TRUE;
	
	//
	// Send Start Condition
	//
	
	ENABLE_START();
	
	WAIT_FOR_OP_COMPLETION();
	
	DISABLE_START();
	
	if (gIsRepeatedStart)
	{
		Status = ProcessTwiStatus(REPEATED_START_TRANSMITTED);
	}
	else
	{
		Status = ProcessTwiStatus(START_TRANSMITTED);
	}
	
	return Status;
}

void
SendStopCondition()
{	
	ENABLE_STOP();
	
	//
	// Send the Stop condition by clearing the TWINT flag
	//
	
	TWCR |= (1 << TWINT);
	
	//
	// TWINT is not set after stop condition.
	// Instead TWSTO bit in TWCR is cleared.
	//
	
	while (!(TWCR & (0x1 << TWSTO)));
}

bool
Transmit(PVOID Buffer, unsigned long Length)
{
	bool Status = TRUE;
	
	unsigned long IndexI;
	
	unsigned char *BytePtr;
	
	TWI_STATUS TransmitStatus;
	
	if (gMode == TWI_MASTER)
	{
		TransmitStatus = DATA_TRANSMITTED_ACK;
	}
	else
	{
		TransmitStatus = SLA_R_DATA_TRANSMITTED_ACK;
	}
	
	//
	// Now transmit the data.
	//
	
	BytePtr = (unsigned char *) Buffer;
	
	for (IndexI = 0; IndexI < Length - 1; IndexI++)
	{
		TWDR = *BytePtr;
		
		WAIT_FOR_OP_COMPLETION();
		
		if (!ProcessTwiStatus(TransmitStatus))
		{
			SendTrace(ERROR_LEVEL, "Transmit - Data Transmission Failed - Status = 0x%x\r\n", (TWSR & TWI_STATUS_MASK));
			
			Status = FALSE;
			
			goto Exit;
		}
		
		BytePtr++;
	}
	
	//
	// In slave mode after transmitting the last byte we will get a NACK from the Master
	//
	
	if (gMode == TWI_SLAVE)
	{
		TransmitStatus = SLA_R_DATA_TRANSMITTED_NACK;
	}
	
	TWDR = *BytePtr;
	
	WAIT_FOR_OP_COMPLETION();
	
	if (!ProcessTwiStatus(TransmitStatus))
	{
		SendTrace(ERROR_LEVEL, "Transmit - Data Transmission Failed - Status = 0x%x\r\n", (TWSR & TWI_STATUS_MASK));
		
		Status = FALSE;
		
		goto Exit;
	}
	
Exit:

	return Status;	
}

bool
Receive(PVOID Buffer, unsigned long Length)
{
	bool Status = TRUE;
	
	unsigned long IndexI;
	
	unsigned char *BytePtr;
	
	TWI_STATUS ReceiveStatus;
	
	if (gMode == TWI_MASTER)
	{
		ReceiveStatus = DATA_RECEIVED_ACK;
	}
	else
	{
		ReceiveStatus = SLA_W_DATA_RECEIVED_ACK;
	}
	
	//
	// Now receive the data.
	//
	
	BytePtr = (unsigned char *) Buffer;
	
	for (IndexI = 0; IndexI < Length - 1; IndexI++)
	{
			
		WAIT_FOR_OP_COMPLETION();
		
		if (!ProcessTwiStatus(ReceiveStatus))
		{
			SendTrace(ERROR_LEVEL, "Receive - Data Reception Failed - Status = 0x%x\r\n", (TWSR & TWI_STATUS_MASK));
			
			Status = FALSE;
			
			goto Exit;
		}
		
		*BytePtr = TWDR;
		
		BytePtr++;
	}
	
	//
	// When receiving the last byte if the receiver is a master then we need to transmit a NACK.
	// If the receiver is a slave we need to transmit a ACK.
	//
	
	if (gMode == TWI_MASTER)
	{
		ENABLE_NACK();
		
		ReceiveStatus = DATA_RECEIVED_NACK;
	}
	
	WAIT_FOR_OP_COMPLETION();
	
	if (!ProcessTwiStatus(ReceiveStatus))
	{
		SendTrace(ERROR_LEVEL, "Receive - Data Reception Failed - Status = 0x%x\r\n", (TWSR & TWI_STATUS_MASK));
		
		Status = FALSE;
		
		goto Exit;
	}
	
	*BytePtr = TWDR;
	
Exit:

	ENABLE_ACK();

	return Status;
}

