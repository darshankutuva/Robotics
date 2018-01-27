#ifndef _OD_SERVOS_H_
#define _OD_SERVOS_H_

#include "Common.h"

typedef enum _OD_SERVO_ROTATION
{

	OD_CLOCKWISE_90			= 222,		// 0.9 milli seconds

	OD_CLOCKWISE_45			= 227,		// 1.2 milli seconds

	OD_NEUTRAL				= 232,		// 1.5 milli seconds

	OD_ANTICLOCKWISE_45		= 236,		// 1.8 milli seconds

	OD_ANTICLOCKWISE_90		= 241		// 2.1 milli seconds
	 
} OD_SERVO_ROTATION;

typedef enum _OD_SERVO
{

	OD_SERVO_FRONT		= 0,

	OD_SERVO_BACK		= 1,

	OD_SERVO_MAX

} OD_SERVO;

void 
InitODServos();

void
TurnODServo(OD_SERVO ODServo,
			TURN_ANGLE TurnAngle);

void
WaitForODServoTurnPulse(unsigned int PulseCount);

#endif
