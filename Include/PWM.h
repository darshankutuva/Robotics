/*
 *  PWM.h
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
 
#ifndef _PWM_H_
#define _PWM_H_

#define PWM_PULSE_DIVS	16

typedef enum _PWM_CHANNEL
{
	PWM_CHANNEL_0			= 0,
	
	PWM_CHANNEL_1			= 1,
	
	PWM_CHANNEL_2			= 2,
	
	PWM_CHANNEL_3			= 3,
	
	PWM_MAX_CHANNELS		= 4

} PWM_CHANNEL;

bool
InitPWM();

void
TurnOffPWM();

bool
PWMChangePulseDuration(PWM_CHANNEL PwmChannel, unsigned char Pulse);

#endif