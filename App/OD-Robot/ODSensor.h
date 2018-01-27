#ifndef _OD_SENSOR_H_
#define _OD_SENSOR_H_

#include "Common.h"

//
// This the maxmimum limit. Crossing this limit indicates 
// that there is obstracle in front of the sensor.
//  

#define OD_SENSOR_MAX_LIMIT		1500

typedef enum _OD_SENSOR 
{

	OD_SENSOR_FRONT = 0,

	OD_SENSOR_BACK = 1

} OD_SENSOR;


unsigned char
CheckForEnoughSpace(OD_SENSOR ODSensor);


TURN_ANGLE
FindEnoughSpace(OD_SENSOR ODSensor);

#endif
