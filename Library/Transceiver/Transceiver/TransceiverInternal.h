/*
 *  TransceiverInternal.h
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

#ifndef _TRANSCEIVER_INTERNAL_
#define _TRANSCEIVER_INTERNAL_

#include "Transceiver.h"

#ifdef ATMEGA_1284P

#include "SPI.h"

#endif


#define CE_LOW()	(PORTC &= ~(1 << PORTC7))

#define CE_HIGH() 	(PORTC |= (1 << PORTC7))

#define MAX_PAYLOAD_LENGTH			32

#define TRANSCEIVER_ADDR_LENGTH		5

//
// Config Register
//

#define CONFIG_OFFSET		0x0

typedef union _CONFIG
{	
	struct  
	{
		unsigned char PRIM_RX		: 1;
	
		unsigned char PWR_UP		: 1;
	
		unsigned char CRCO			: 1;
	
		unsigned char EN_CRC		: 1;
	
		unsigned char MASK_MAX_RT	: 1;
	
		unsigned char MASK_TX_DS	: 1;
	
		unsigned char MASK_RX_DR	: 1;
	
		unsigned char Rsvd			: 1;
	};
	
	unsigned char AsUINT8;

} CONFIG, *PCONFIG;

typedef enum _DATA_PIPE
{
	DATA_PIPE_0		= 0x0,
	
	DATA_PIPE_1		= 0x1,
	
	DATA_PIPE_2		= 0x2,
	
	DATA_PIPE_3		= 0x3,
	
	DATA_PIPE_4		= 0x4,
	
	DATA_PIPE_5		= 0x5
	
} DATA_PIPE;

//
// Enable Auto Acknowledgment Register
//

#define EN_AA_OFFSET		0x1

typedef union _EN_DATA_PIPE
{
	struct  
	{
		unsigned char DATA_PIPE		: 6;
	
		unsigned char Rsvd			: 2;	
	};
	
	unsigned char AsUINT8;

} EN_AA, *PEN_AA, EN_RX_ADDR, *PEN_RX_ADDR;

//
// Enable RX Address Register
//

#define EN_RX_ADDR_OFFSET	0x2

//
// Setup Address Widths Register
//

#define SETUP_AW_OFFSET		0x3

typedef enum _ADDRESS_WIDTH
{
	AW_ILLEGAL		= 0x0,
	
	AW_3_BYTES		= 0x1,
	
	AW_4_BYTES		= 0x2,
	
	AW_5_BYTES		= 0x3
	
} ADDRESS_WIDTH;

//
// Setup Automatic Retransmission Register
//

#define SETUP_RETR_OFFSET	0x4

typedef union _SETUP_RETR
{
	struct
	{
		unsigned char ARC	: 4;
		
		unsigned char ARD	: 4;
	};
	
	unsigned char AsUINT8;

} SETUP_RETR, *PSETUP_RETR;

//
// RF Channel Register
//

#define RF_CH_OFFSET		0x5

//
// RF_SETUP Register
//

#define RF_SETUP_OFFSET			0x6

typedef union _RF_SETUP
{
	struct  
	{
		unsigned char Rsvd0			: 1;
	
		unsigned char RF_PWR		: 2;
	
		unsigned char RF_DR_HIGH	: 1;
	
		unsigned char PLL_LOCK		: 1;
	
		unsigned char RF_DR_LOW		: 1;
	
		unsigned char Rsvd1			: 1;
	
		unsigned char CONT_WAVE		: 1;
	};
	
	unsigned char AsUINT8;
	
} RF_SETUP, *PRF_SETUP;

typedef enum _RF_PWR
{
	Minus18dBm,
	
	Minus12dBm,
	
	Minus6dBm,
	
	ZerodBm
		
} RF_PWR;

//
// STATUS Register
//

#define STATUS_OFFSET		0x7

typedef union _STATUS_REG
{
	struct  
	{
		unsigned char TX_FULL	: 1;
	
		unsigned char RX_P_NO	: 3;
	
		unsigned char MAX_RT	: 1;
	
		unsigned char TX_DS		: 1;
	
		unsigned char RX_DR		: 1;
	
		unsigned char Rsvd		: 1;
	};
	
	unsigned char AsUINT8;
	
} STATUS_REG, *PSTATUS_REG;

//
// Observe TX Register
//

#define OBSERVE_TX_OFFSET		0x8

typedef union _OBSERVE_TX
{
	struct
	{
		unsigned char ARC_CNT	: 4;
		
		unsigned char PLOS_CNT	: 4;
	};
	
	unsigned char AsUINT8;

} OBSERVE_TX, *POBSERVE_TX;

//
// RPD Register
//

#define RPD_OFFSET				0x9

typedef union _RPD
{
	struct  
	{
		unsigned char RPD		: 1;
	
		unsigned char Rsvd		: 7;
	};
	
	unsigned char AsUINT8;
	
} RPD, *PRPD;

//
// RX_ADDR_P0, RX_ADDR_P1
//

#define RX_ADDR_P0_OFFSET	0xa

#define RX_ADDR_P1_OFFSET	0xb

//
// RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
//

typedef unsigned char BYTE_ADDR, *PBYTE_ADDR;

#define RX_ADDR_P2_OFFSET	0xc

#define RX_ADDR_P3_OFFSET	0xd

#define RX_ADDR_P4_OFFSET	0xe

#define RX_ADDR_P5_OFFSET	0xf

//
// TX_ADDR
//

#define TX_ADDR_OFFSET		0x10

//
// RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
//

#define RX_PW_P0_OFFSET		0x11

#define RX_PW_P1_OFFSET		0x12

#define RX_PW_P2_OFFSET		0x13

#define RX_PW_P3_OFFSET		0x14

#define RX_PW_P4_OFFSET		0x15

#define RX_PW_P5_OFFSET		0x16

//
// FIFO_STATUS
//

#define FIFO_STATUS_OFFSET			0x17

typedef union _FIFO_STATUS
{
	struct  
	{
		unsigned char RX_EMPTY	: 1;
	
		unsigned char RX_FULL	: 1;
	
		unsigned char Rsvd0		: 2;
	
		unsigned char TX_EMPTY	: 1;
	
		unsigned char TX_FULL	: 1;
	
		unsigned char TX_REUSE	: 1;
	
		unsigned char Rsvd1		: 1;
	};
	
	unsigned char AsUINT8;

} FIFO_STATUS, *PFIFO_STATUS;

//
// Payload
//

typedef struct _PAYLOAD
{
	unsigned char Bytes[32];

} PAYLOAD;

typedef PAYLOAD ACK_PLD;

typedef PAYLOAD TX_PLD;

typedef PAYLOAD RX_PLD;

//
// DYNPD Register
//

#define DYNPD_OFFSET		0x1c

typedef union _DYNPD
{
	struct  
	{
		unsigned char DPL_P0	: 1;
	
		unsigned char DPL_P1	: 1;
	
		unsigned char DPL_P2	: 1;
	
		unsigned char DPL_P3	: 1;
	
		unsigned char DPL_P4	: 1;
	
		unsigned char DPL_P5	: 1;
	
		unsigned char Rsvd		: 2;
	};
	
	unsigned char AsUINT8;
	
} DYNPD, *PDYNPD;

//
// FEATURE Register
//

#define FEATURE_OFFSET		0x1D

typedef union _FEATURE
{
	struct  
	{
		unsigned char EN_DYN_ACK	: 1;
	
		unsigned char EN_ACK_PAY	: 1;
	
		unsigned char EN_DPL		: 1;
	
		unsigned char Rsvd			: 4;
	};
	
	unsigned char AsUINT8;
	
} FEATURE, *PFEATURE;

typedef enum _FIFO_TYPE
{
	RX_FIFO,
	
	TX_FIFO

} FIFO_TYPE;

//
// Commands
//

#define R_REGISTER				0x0

#define W_REGISTER				0x20

#define R_RX_PAYLOAD			0x61

#define W_TX_PAYLOAD			0xa0

#define FLUSH_TX				0xe1

#define FLUSH_RX				0xe2

#define REUSE_TX_PL				0xe3

#define R_RX_PL_WID				0x60

#define W_ACK_PAYLOAD			0xa8

#define W_TX_PAYLOAD_NO_ACK		0xb0

#define NOP						0xff

//
// Private Functions
//

void
TCFlushFIFO(FIFO_TYPE FIFOType);

bool
TCWriteReg(unsigned char RegOffset, PVOID Value, unsigned char Length);

bool
TCReadReg(unsigned char RegOffset, PVOID Value, unsigned char Length);

bool
TCTransmit(unsigned char *CommandStatus, PVOID Value, unsigned char Length);

bool
TCReceive(unsigned char *CommandStatus, PVOID Value, unsigned char Length);

void
TCIntCallback(PVOID CallbackContext);

//
// Transfer Wrapper Functions
//

void
TCInitTransfer();

#ifdef ATMEGA_1284P

	#define TCBeginTransfer() (SPIBeginTransfer())

	#define TCEndTransfer() (SPIEndTransfer())

	#define TCTransferSync(TransmitBuffer, ReceiveBuffer, Length) (SPIMasterTransfer(TransmitBuffer, ReceiveBuffer, Length))

#else

	#define TCBeginTransfer()
	
	#define TCEndTransfer()
	
	#define TCTransferSync(TransmitBuffer, ReceiveBuffer, Length)

#endif

#endif 