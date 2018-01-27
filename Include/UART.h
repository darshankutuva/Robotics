/*
 *  UART.h
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
 
#ifndef _UART_H_
#define _UART_H_

#include <avr/io.h>
#include "Common.h"

typedef enum _UART_NUMBER
{

	UART_ZERO		= 0x0,

	UART_ONE		= 0x1,

	MAX_UART		= 0x2

} UART_NUMBER;

typedef enum _UART_MODE
{

	NORMAL_MODE							= 0x0,
	
	MULTI_PROCESSOR_COMMUNICATION_MODE	= 0x1

	//DOUBLE_SPEED_MODE	= 0x8

} UART_MODE;

typedef enum _BAUD_RATE
{
	BR_1200		= 416,
	
	BR_2400		= 207,
	
	BR_4800		= 103,
	
	BR_9600		= 51,
	
	BR_14_4K	= 34,
	
	BR_19_2K	= 25,
	
	BR_28_8K	= 16,
	
	BR_38_4K	= 12,
	
	BR_57_6K	= 8,
	
	BR_76_8K	= 6,
	
	BR115_2K	= 3,
	
	BR230_4K	= 1,
	
	BR250K		= 1,
	
	BR0_5M		= 0
	
} BAUD_RATE;

bool
InitUARTSync(UART_NUMBER UARTNumber, BAUD_RATE BaudRate, UART_MODE UARTMode);

unsigned long
UARTTransmitAddress(UART_NUMBER UARTNumber, unsigned char Address);

unsigned long
UARTTransmitSync(UART_NUMBER UARTNumber, PVOID TransmitValue, unsigned long Length);

unsigned long
UARTReceiveSync(UART_NUMBER UARTNumber, PVOID ReceiveValue, unsigned long Length);

unsigned long
UARTReceiveASync(UART_NUMBER UARTNumber, PVOID ReceiveValue, unsigned long Length, unsigned char WaitTimeInMs);

typedef void (*PRCI_CALLBACK) (UART_NUMBER UARTNumber, PVOID pCallbackContext);

typedef void (*PTCI_CALLBACK) (UART_NUMBER UARTNumber, PVOID pCallbackContext);

bool
UARTEnableDisableReceiver(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable);

bool
UARTEnableDisableTransmitter(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable);

bool
UARTEnableDisableMPCM(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable);

bool
UARTEnableDisableReceiveCompleteInterrupt(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable, PRCI_CALLBACK pRCICallback, PVOID pCallbackContext);

bool
UARTEnableDisableTransmitCompleteInterrupt(UART_NUMBER UARTNumber, ENABLE_DISABLE EnableDisable, PTCI_CALLBACK pTCICallback, PVOID pCallbackContext);

unsigned long
UARTTransmit(UART_NUMBER UARTNumber, PVOID ReceiveValue, unsigned long Length, bool IsAddress);

unsigned long
UARTReceive(UART_NUMBER UARTNumber, PVOID ReceiveValue, unsigned long Length, unsigned char WaitTimeInMs);

#endif
