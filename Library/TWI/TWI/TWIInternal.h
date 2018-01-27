/*
 * TWIInternal.h
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

#ifndef TWIINTERNAL_H_
#define TWIINTERNAL_H_

#include "Common.h"

#define BIT_RATE_VALUE	32

#define TWI_STATUS_MASK 0xF8

#define TWCR_MASK		0x7F

#define WAIT_FOR_OP_COMPLETION() {		\
										\
	TWCR |= (0x1 << TWINT);				\
										\
	while (!(TWCR & (0x1 << TWINT)));	\
}

#define TWCR_MASKED_VALUE()	(TWCR & TWCR_MASK)
												
#define ENABLE_ACK() (TWCR = TWCR_MASKED_VALUE() | (0x1 << TWEA))
												
#define ENABLE_NACK()	(TWCR = TWCR_MASKED_VALUE() & ~(0x1 << TWEA))

#define ENABLE_START() (TWCR = TWCR_MASKED_VALUE() | (0x1 << TWSTA))

#define DISABLE_START() (TWCR = TWCR_MASKED_VALUE() & ~(0x1 << TWSTA))

#define ENABLE_STOP() (TWCR = TWCR_MASKED_VALUE() | (0x1 << TWSTO))

#define DATA_OUT() (DDRC |= (0x1 << PORTC1))

#define DATA_IN() (DDRC &= ~(0x1 << PORTC1))

#define ProcessTwiStatus(ValidStatus) (((TWSR & TWI_STATUS_MASK) == ValidStatus) ? TRUE : FALSE)


typedef enum _TWI_STATUS
{
	START_TRANSMITTED				= 0x8,

	REPEATED_START_TRANSMITTED		= 0x10,

	SLA_W_TRANSMITTED_ACK			= 0x18,

	SLA_W_TRANSMITTED_NACK			= 0x20,

	DATA_TRANSMITTED_ACK			= 0x28,

	DATA_TRANSMITTED_NACK			= 0x30,

	ARB_LOST						= 0x38,

	SLA_R_TRANSMITTED_ACK			= 0x40,
	
	SLA_R_TRANSMITTED_NACK			= 0x48,

	DATA_RECEIVED_ACK				= 0x50,

	DATA_RECEIVED_NACK				= 0x58,

	SLA_W_RECEIVED_ACK				= 0x60,

	ARB_LOST_SLA_W_RECEIVED_ACK		= 0x68,

	GEN_RECEIVED_ACK				= 0x70,

	ARB_LOST_GEN_RECEIVED_ACK		= 0x78,

	SLA_W_DATA_RECEIVED_ACK			= 0x80,

	SLA_W_DATA_RECEIVED_NACK		= 0x88,

	GEN_DATA_RECEIVED_ACK			= 0x90,

	GEN_DATA_RECEIVED_NACK			= 0x98,

	STOP_REPEATED_START_RECEIVED	= 0xa0,

	SLA_R_RECEIVED_ACK				= 0xa8,

	ARB_LOST_SLA_R_RECEIVED_ACK		= 0xb0,

	SLA_R_DATA_TRANSMITTED_ACK		= 0xb8,

	SLA_R_DATA_TRANSMITTED_NACK		= 0xc0,

	SLA_R_LAST_DATA_TRANSMITTED_ACK	= 0xc8

} TWI_STATUS;

typedef union _TWI_STATUS_REG
{
	struct  
	{
		unsigned char PreScalar : 2;
	
		unsigned char Rsvd : 1;
	
		unsigned char Status : 5;
	};
	
	unsigned char AsUINT8;
	
} TWI_STATUS_REG, *PTWI_STATUS_REG;


bool
TwiMasterTransmit(PVOID Buffer,
				  unsigned long Length,
				  PTWI_ADDRESS Address,
				  bool SendStop);
				  
bool
TwiMasterReceive(PVOID Buffer,
				 unsigned long Length,
				 PTWI_ADDRESS Address,
				 bool SendStop);
				 
bool
SendStartCondition();

void
SendStopCondition();

bool
Transmit(PVOID Buffer, unsigned long Length);

bool
Receive(PVOID Buffer, unsigned long Length);

#endif /* TWIINTERNAL_H_ */