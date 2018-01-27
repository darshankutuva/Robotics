/*
 * SPIInternal.h
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

#ifndef SPIINTERNAL_H_
#define SPIINTERNAL_H_


#include "Common.h"


#define SS_LOW()		(PORTB &= ~(1 << PORTB4))

#define SS_HIGH() 		(PORTB |= (1 << PORTB4))

#define IS_SS_HIGH()	(PORTB & (1 << PORTB4))

#define SPCR_CLOCK_RATE_MASK 	0x3

#define SPSR_CLOCK_RATE_MASK	0x4

#endif /* SPIINTERNAL_H_ */