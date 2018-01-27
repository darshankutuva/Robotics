#ifndef _POWER_GATING_H_
#define _POWER_GATING_H_

#include "ODSensor.h"

#include "ODServos.h"

typedef enum _POWER_STATUS
{

	POWER_OFF = 0,

	POWER_ON = 1

} POWER_STATUS;

void 
InitPowerGatingLogic();

void 
TurnOnOffODSensor(OD_SENSOR ODSensor, 
				  POWER_STATUS PowerStatus);

void 
TurnOnOffODServo(OD_SERVO ODServo, 
				 POWER_STATUS PowerStatus);

#endif
