/*
 *  Transceiver.h
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

#ifndef _TRANSCEIVER_H_
#define _TRANSCEIVER_H_

#include "Common.h"

//
// Interrupt Callback Function
//

typedef void (*PTCInterruptCallback) (PVOID);

//
// Transceiver Mode
//

typedef enum _TRANSCEIVER_MODE
{
	RECEIVER	= 0,
	
	TRANSMITTER	= 1
	
} TRANSCEIVER_MODE;

//
// 40 BIT ADDRESS
//

typedef union _TRANSCEIVER_ADDR
{
	struct
	{
		unsigned long long Address	: 40;
		
		unsigned long long Rsvd		: 24;
	};
	
	unsigned long long AsULongLong;
	
} TRANSCEIVER_ADDR, *PTRANSCEIVER_ADDR;

//
// Auto Retry Delay Enumeration
//

typedef enum _AUTO_RETRY_DELAY
{
	ARD_250_MS		= 0x0,
	
	ARD_500_MS		= 0x1,
	
	ARD_750_MS		= 0x2,
	
	ARD_1000_MS		= 0x3,
	
	ARD_1250_MS		= 0x4,
	
	ARD_1500_MS		= 0x5,
	
	ARD_1750_MS		= 0x6,
	
	ARD_2000_MS		= 0x7,
	
	ARD_2250_MS		= 0x8,
	
	ARD_2500_MS		= 0x9,
	
	ARD_2750_MS		= 0xa,
	
	ARD_3000_MS		= 0xb,
	
	ARD_3250_MS		= 0xc,
	
	ARD_3500_MS		= 0xd,
	
	ARD_3750_MS		= 0xe,
	
	ARD_4000_MS		= 0xf
	
} AUTO_RETRY_DELAY;

//
// Auto Retry Count Enumeration
//

typedef enum _AUTO_RETRY_COUNT
{
	ARC_0		= 0x0,
	
	ARC_1		= 0x1,
	
	ARC_2		= 0x2,
	
	ARC_3		= 0x3,
	
	ARC_4		= 0x4,
	
	ARC_5		= 0x5,
	
	ARC_6		= 0x6,
	
	ARC_7		= 0x7,
	
	ARC_8		= 0x8,
	
	ARC_9		= 0x9,
	
	ARC_10		= 0xa,
	
	ARC_11		= 0xb,
	
	ARC_12		= 0xc,
	
	ARC_13		= 0xd,
	
	ARC_14		= 0xe,
	
	ARC_15		= 0xf
	
} AUTO_RETRY_COUNT;

//
// Payload Length
//

typedef union _PAYLOAD_LENGTH
{
	struct
	{
		unsigned char PayloadLength : 6;
		
		unsigned char Rsvd : 2;
	};
	
	unsigned char AsUINT8;
	
} PAYLOAD_LENGTH, *PPAYLOAD_LENGTH;

//
// RF Channel
//

typedef union _RF_CH
{
	struct
	{
		unsigned char RFCH		: 7;
		
		unsigned char Rsvd		: 1;
	};
	
	unsigned char AsUINT8;
	
} RF_CH, *PRF_CH;

//
// Transceiver Initialization Parameter
//

typedef struct _TRANSCEIVER_INIT_PARAMS
{
	TRANSCEIVER_MODE		Mode;
	
	TRANSCEIVER_ADDR		MyAddress;
	
	TRANSCEIVER_ADDR		DestAddress;
	
	AUTO_RETRY_COUNT		AutoRetryCount;
	
	AUTO_RETRY_DELAY		AutoRetryDelay;
	
	RF_CH					RFChannel;
	
	PTCInterruptCallback	MaxRTCallback;
	
	PVOID					MaxRTCallbackContext;
	
	PTCInterruptCallback	RxDRCallback;
	
	PVOID					RxDRCallbackContext;
	
	PTCInterruptCallback	TxDSCallback;
	
	PVOID					TxDSCallbackContext;
	
	bool					IsAutoAcknowledgeEnabled;
	
} TRANSCEIVER_INIT_PARAMS, *PTRANSCEIVER_INIT_PARAMS;

//
// Public Functions
//

bool
InitTransceiver (PTRANSCEIVER_INIT_PARAMS InitParams);

unsigned char
TCGetLostPacketCount();

unsigned char
TCGetRetransmitPacketCount();

void
TCGetReceivedPayloadLength(PPAYLOAD_LENGTH Length);

bool
TCIsReceivePayloadAvailable();

bool
TCReceivePayload(PVOID Payload, PPAYLOAD_LENGTH Length);

bool
TCTransmitPayload(PVOID Payload, PPAYLOAD_LENGTH Length);

bool
TCTransmitAckPayload(PVOID Payload, PPAYLOAD_LENGTH Length);

#endif