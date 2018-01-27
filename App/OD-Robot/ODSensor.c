#define F_CPU 1000000UL

#include <avr/io.h>

#include <util/delay.h>

#include "ODSensor.h"

#include "PowerGating.h"

#include "ADC.h"

#include "UART-Trace.h"

unsigned char
CheckForEnoughSpace(OD_SENSOR ODSensor)
{
	unsigned int ADValue;

	TurnOnOffODSensor(ODSensor, POWER_ON);

	_delay_ms(1000);

	switch(ODSensor)
	{
		case OD_SENSOR_FRONT:
		{

			ADValue = ReadDataFromAD(AD_CHANNEL_0);

			break;

		}
		case OD_SENSOR_BACK:
		{

			ADValue = ReadDataFromAD(AD_CHANNEL_1);

			break;

		}
		default:
		{

			return 0;

		}
	}

	TurnOnOffODSensor(ODSensor, POWER_OFF);

	if (ADValue >= OD_SENSOR_MAX_LIMIT)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}

TURN_ANGLE
FindEnoughSpace(OD_SENSOR ODSensor)
{

	unsigned short ADValue;

	unsigned short ADValueMin = OD_SENSOR_MAX_LIMIT;

	TURN_ANGLE ODServoTurnAngle = TURN_ANGLE_MAX;

	TURN_ANGLE TurnAngle;

	AD_CHANNEL ADChannel = AD_CHANNEL_0;;

	OD_SERVO ODServo = OD_SERVO_FRONT;

	switch(ODSensor)
	{
		case OD_SENSOR_FRONT:
		{

			ODServo = OD_SERVO_FRONT;

			ADChannel = AD_CHANNEL_0;

			break;
		}
		case OD_SENSOR_BACK:
		{

			ODServo = OD_SERVO_BACK;

			ADChannel = AD_CHANNEL_1;

			break;
		}
		default:
		{

			goto Exit;

		}
	}

	TurnOnOffODSensor(ODSensor, POWER_ON);

	for (TurnAngle = LEFT_90; TurnAngle < TURN_ANGLE_MAX; TurnAngle++)
	{

		TurnODServo(ODServo, TurnAngle);

		ADValue = ReadDataFromAD(ADChannel);

		if ((ADValue < OD_SENSOR_MAX_LIMIT) && (ADValue < ADValueMin))
		{

			ADValueMin = ADValue;

			ODServoTurnAngle = TurnAngle;

		}
	}

	TurnODServo(ODServo, NEUTRAL);

	TurnOnOffODSensor(ODSensor, POWER_OFF);

Exit:

	return ODServoTurnAngle;
}
