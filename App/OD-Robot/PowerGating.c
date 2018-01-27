#define F_CPU 1000000UL

#include <avr/io.h>

#include "PowerGating.h"

#include "UART-Trace.h"

void 
InitPowerGatingLogic()
{
	//
	// Set pins PC0, PC1, PC2, PC3 to output.
	//
	// PC0 - Controls power to Forward Object Detection Sensor.
	// PC1 - Controls power to Forward Object Detection Servo.
	// PC2 - Controls power to Backward Object Detection Sensor.
	// PC3 - Controls power to Backward Object Detection Servo.
	//	

	DDRC |= ((1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3));

	//
	// To start with turn off everything.
	//
	
	TurnOnOffODSensor(OD_SENSOR_FRONT, POWER_OFF);

	TurnOnOffODSensor(OD_SENSOR_BACK, POWER_OFF);

	TurnOnOffODServo(OD_SERVO_FRONT, POWER_OFF);

	TurnOnOffODServo(OD_SERVO_BACK, POWER_OFF);

}

void 
TurnOnOffODSensor(OD_SENSOR ODSensor, 
				  POWER_STATUS PowerStatus)
{

	switch (ODSensor)
	{

		case OD_SENSOR_FRONT:
		{

			switch (PowerStatus)
			{

				case POWER_OFF:
				{

					PORTC = PORTC & ~(1 << PC0);

					break;

				}

				case POWER_ON:
				{
			
					PORTC = PORTC | (1 << PC0);

					break;

				}

			}

			break;

		}

		case OD_SENSOR_BACK:
		{

			switch (PowerStatus)
			{

				case POWER_OFF:
				{

					PORTC = PORTC & ~(1 << PC2);

					break;

				}

				case POWER_ON:
				{
			
					PORTC = PORTC | (1 << PC2);

					break;

				}

			}
			
			break;

		}

	}

}

void 
TurnOnOffODServo(OD_SERVO ODServo, 
				 POWER_STATUS PowerStatus)
{

	switch (ODServo)
	{

		case OD_SERVO_FRONT:
		{

			switch (PowerStatus)
			{

				case POWER_OFF:
				{

					//SendTrace(7);

					PORTC = PORTC & ~(1 << PC1);

					break;

				}

				case POWER_ON:
				{

					//SendTrace(8);			

					PORTC = PORTC | (1 << PC1);

					break;

				}

			}

			break;

		}

		case OD_SERVO_BACK:
		{

			switch (PowerStatus)
			{

				case POWER_OFF:
				{

					PORTC = PORTC & ~(1 << PC3);

					break;

				}

				case POWER_ON:
				{
			
					PORTC = PORTC | (1 << PC3);

					break;

				}

			}

			break;
		}

		default:
		{

			break;

		}

	}

}
