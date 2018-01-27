#ifndef _COMMON_H_
#define _COMMON_H_

#define PWM_PULSES_1		1

#define PWM_PULSES_11		11	// For 45 degree turn

#define PWM_PULSES_24		24	// For 90 degree turn


typedef enum _TURN_ANGLE
{

	NEUTRAL		= 0,	

	LEFT_90 	= 1,
	
	LEFT_45 	= 2,

	RIGHT_45 	= 3,

	RIGHT_90	= 4,
	
	TURN_ANGLE_MAX	

} TURN_ANGLE;

#endif
