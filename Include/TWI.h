/*
 * TWI.h
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

#ifndef _TWI_H_
#define _TWI_H_

#include "Common.h"


typedef enum _TWI_MODE
{
	TWI_SLAVE,

	TWI_MASTER

} TWI_MODE;

typedef union _TWI_ADDRESS
{
	struct
	{
		unsigned char RW_EnGenCall : 1;
		
		unsigned char Address : 7;
	};
	
	unsigned char AsUINT8;
	
} TWI_ADDRESS, *PTWI_ADDRESS;

typedef union _TWI_ADDRESS_MASK
{		
	struct
	{
		unsigned char Rsvd : 1;
		
		unsigned char Mask : 7;		
	};
	
	unsigned char AsUINT8;
	
} TWI_ADDRESS_MASK, *PTWI_ADDRESS_MASK;


bool
InitTWI(TWI_MODE Mode, PTWI_ADDRESS Address, PTWI_ADDRESS_MASK AddressMask);

bool
TwiTransmit(PVOID Buffer, 
			unsigned long Length, 
			PTWI_ADDRESS Address,
			bool SendStop);

bool
TwiReceive(PVOID Buffer,
		   unsigned long Length,
		   PTWI_ADDRESS Address,
		   bool SendStop);

#endif
