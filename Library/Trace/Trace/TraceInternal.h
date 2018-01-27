/*
 * TraceInternal.h
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


#ifndef TRACEINTERNAL_H_
#define TRACEINTERNAL_H_

#include "Common.h"

typedef enum _VALUE_TYPE
{

	DECIMAL_VALUE		= 0x0,

	HEX_VALUE			= 0x1

} VALUE_TYPE;

void
SendStringValue(char *String);

void
SendHexValue(unsigned long long Value);

void
SendDecimalValue(unsigned long long Value);

void
SendSignedDecimalValue(long long Value);

void
SendDoubleValue(double Value);


#endif /* TRACEINTERNAL_H_ */