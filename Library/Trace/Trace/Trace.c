/*
 * Trace.c
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
 
#include <stdarg.h>

#include <string.h>

#include "Trace.h"

#include "TraceInternal.h"

#include "UART.h"

#define MAX_DECIMAL_CHARS		256

bool gTraceInitialized = FALSE;

unsigned char gTraceLevel;

#ifdef DEBUG

void
InitTrace(unsigned long ProcFrequency, unsigned char TraceLevel)
{
	if (!gTraceInitialized)
	{

		gTraceLevel = TraceLevel;

		InitUARTSync(UART_ZERO, BR_9600, NORMAL_MODE);	
		
		gTraceInitialized = TRUE;
	
	}
}

void
SendTrace(TRACE_LEVEL TraceLevel, const char *String, ...)
{

	bool IsLong = FALSE;
	
	bool IsLongLong = FALSE;
	
	char Character;

	va_list ArgList;

	if (!gTraceInitialized || !(TraceLevel & gTraceLevel))
	{
		return;
	}

	va_start(ArgList, String);

	while (*String != '\0')
	{

		if (*String == '%' || IsLong || IsLongLong)
		{

			if (*String == '%')
			{
				String++;
			}

			switch (*String)
			{
				case 'c':
				{

					Character = va_arg(ArgList, int);
					
					UARTTransmitSync(UART_ZERO, (PVOID) &Character, 1);

					break;

				}
				case 'd':
				{

					if (IsLong)
					{

						SendSignedDecimalValue(va_arg(ArgList, long));

						IsLong = FALSE;

					}
					else if (IsLongLong)
					{
						SendSignedDecimalValue(va_arg(ArgList, long long));

						IsLongLong = FALSE;
					}
					else
					{

						SendSignedDecimalValue(va_arg(ArgList, int));

					}

					break;
				}
				case 'u':
				{

					if(IsLong)
					{

						SendDecimalValue(va_arg(ArgList, unsigned long));
						
						IsLong = FALSE;
					
					}
					else if(IsLongLong)
					{
						SendDecimalValue(va_arg(ArgList, unsigned long long));
						
						IsLongLong = FALSE;
					}
					else
					{

						SendDecimalValue(va_arg(ArgList, unsigned int));

					}

					break;
				}
				case 'x':
				{

					if (IsLong)
					{

						SendHexValue(va_arg(ArgList, unsigned long));

						IsLong = FALSE;

					}
					else if (IsLongLong)
					{
						SendHexValue(va_arg(ArgList, unsigned long long));
						
						IsLongLong = FALSE;
					}
					else
					{

						SendHexValue(va_arg(ArgList, unsigned int));

					}

					break;
				}
				case 'l':
				{

					if (IsLong)
					{
						IsLongLong = TRUE;
						
						IsLong = FALSE;
					}
					else
					{
						IsLong = TRUE;
					}

					break;
				}
				case 's':
				{
					
					SendStringValue(va_arg(ArgList, char *)); 

					break;
				}
				case 'f':
				{
					if(IsLong)
					{
						IsLong = FALSE;
					}
					
					SendDoubleValue(va_arg(ArgList, double));
					
					break;
				}
				default:
				{
					break;
				}
			}			

		}
		else
		{

			UARTTransmitSync(UART_ZERO, (PVOID) String, 1);
				
		}

		String++;
		
	}
	
	va_end(ArgList);
	
}

#else

void
InitTrace(unsigned long ProcFrequency, unsigned char TraceLevel)
{
	//
	// Stub Function
	//
}

void
SendTrace(TRACE_LEVEL TraceLevel, const char *String, ...)
{
	//
	// Stub Function
	//
}

#endif

void
SendStringValue(char *String)
{

	unsigned long Length = 0;
	
	Length = strlen(String);
	
	UARTTransmitSync(UART_ZERO, (PVOID) String, Length);
	
}

void
SendHexValue(unsigned long long Value)
{

	unsigned char TransmitChar[MAX_DECIMAL_CHARS];

	unsigned char Count = 0;


	do
	{

		if (Value < 16)
		{
			TransmitChar[Count] = Value;

			Value = 0;
		}
		else
		{
			TransmitChar[Count] = Value % 16;

			Value = Value / 16;

		}
		
		if (TransmitChar[Count] > 9)
		{
			TransmitChar[Count] = TransmitChar[Count] + ASCII_HEX_CONVERSION;
		}
		else
		{
			TransmitChar[Count] = TransmitChar[Count] + ASCII_DECIMAL_CONVERSION;
		}
		
		Count++;

	} while (Value != 0);

	while (Count != 0)
	{
		UARTTransmitSync(UART_ZERO, (PVOID) &TransmitChar[Count - 1], 1);
		
		Count--;
	}
}


void
SendDecimalValue(unsigned long long Value)
{

	unsigned char TransmitChar[MAX_DECIMAL_CHARS];

	unsigned char Count = 0;


	do
	{

		if (Value < 10)
		{
			TransmitChar[Count] = Value;

			Value = 0;
		}
		else
		{
			TransmitChar[Count] = Value % 10;

			Value = Value / 10;

		}
		
		TransmitChar[Count] = TransmitChar[Count] + ASCII_DECIMAL_CONVERSION;
		
		Count++;

	} while (Value != 0);

	while (Count != 0)
	{
		UARTTransmitSync(UART_ZERO, (PVOID) &TransmitChar[Count - 1], 1);
		
		Count--;
	}	
}

void
SendDoubleValue(double Value)
{
	
	long long IntPart;
	
	unsigned char FracPart;
	
	unsigned char Dot = '.';
	
	unsigned char Minus = '-';
	
	if (Value < 0)
	{
		
		UARTTransmitSync(UART_ZERO, (PVOID) &Minus, 1);
		
		Value = Value * -1;
	}
	
	IntPart = (long long) Value;
		
	Value = Value - IntPart;
	
	SendDecimalValue(IntPart);
	
	UARTTransmitSync(UART_ZERO, (PVOID) &Dot, 1);
	
	while (Value != 0)
	{
		FracPart = (Value * 10);
		
		Value = (Value * 10) - FracPart;
		
		FracPart = FracPart + ASCII_DECIMAL_CONVERSION;
		
		UARTTransmitSync(UART_ZERO, (PVOID) &FracPart, 1);
		
	}
	
	//
	// Print 0
	//
	
	FracPart = ASCII_DECIMAL_CONVERSION;
	
	UARTTransmitSync(UART_ZERO, (PVOID) &FracPart, 1);
}

void
SendSignedDecimalValue(long long Value)
{
	unsigned char TransmitChar[MAX_DECIMAL_CHARS];

	unsigned char Count = 0;
	
	unsigned char Minus = '-';

	//
	// If the value is negative, print - sign and make the value positive.
	//

	if (Value < 0)
	{
		UARTTransmitSync(UART_ZERO, (PVOID) &Minus, 1);

		Value *= -1;
	}

	do
	{

		if (Value < 10)
		{
			TransmitChar[Count] = Value;

			Value = 0;
		}
		else
		{
			TransmitChar[Count] = Value % 10;

			Value = Value / 10;

		}
	
		TransmitChar[Count] = TransmitChar[Count] + ASCII_DECIMAL_CONVERSION;
		
		Count++;

	} while (Value != 0);

	while (Count != 0)
	{
		UARTTransmitSync(UART_ZERO, (PVOID) &TransmitChar[Count - 1], 1);
		
		Count--;
	}
}

