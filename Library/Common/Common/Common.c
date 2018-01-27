/*
 *  Common.c
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

#include <avr/io.h>

long
RoundLong(double Value)
{
	long IntPart;
	
	double FracPart;
	
	IntPart = (long) Value;
	
	FracPart = Value - IntPart;
	
	if (FracPart < 0.5)
	{
		return IntPart;
	}
	else
	{
		if (Value < 0)
		{
			return IntPart - 1;
		}
		else
		{
			return IntPart + 1;
		}
	}
}