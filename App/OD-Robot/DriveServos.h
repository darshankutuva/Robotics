#ifndef _DRIVE_SERVOS_H_
#define _DRIVE_SERVOS_H_

#include "Common.h"

typedef enum _DRIVE_SERVO_ROTATION
{

	DS_CLOCKWISE_90			= 235,

	DS_NEUTRAL				= 232,

	DS_ANTICLOCKWISE_90		= 228

} DRIVE_SERVO_ROTATION;

typedef enum _TURN_DIRECTION
{

	FORWARD		= 0,

	BACKWARD	= 1

} TURN_DIRECTION;

void 
InitDriveServos();

void
DriveRobotForward();

void
DriveRobotBackward();

void
StopRobot();

void 
TurnRobot(TURN_ANGLE TurnAngle, 
		  TURN_DIRECTION TurnDirection);

void
WaitForRobotTurnPulse(unsigned int PulseCount);

#endif
