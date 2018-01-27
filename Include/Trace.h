/*
 *  Trace.h
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

#ifndef _UARTTRACE_H_
#define _UARTTRACE_H_

#include "Common.h"

#define ASCII_DECIMAL_CONVERSION	48

#define ASCII_HEX_CONVERSION		87

typedef enum _TRACE_LEVEL
{

	ERROR_LEVEL		= 0x1,

	WARNING_LEVEL	= 0x2,

	INFO_LEVEL		= 0x4

} TRACE_LEVEL;

void
InitTrace(unsigned long ProcFrequency, unsigned char TraceLevel);

void
SendTrace(TRACE_LEVEL TraceMask, const char * String, ...);

#endif
