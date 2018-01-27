#define F_CPU 1000000UL

#include <avr/io.h>

#include <util/delay.h>

#include "OD-Robot.h"

void 
InitODRobot()
{

	InitPowerGatingLogic();

	InitUART();

	InitSPI();

	InitODServos();

	InitDriveServos();

}

void 
StartODRobot()
{

	/*unsigned int ADValue;

	TurnOnOffODSensor(OD_SENSOR_FRONT, POWER_ON);

	while (1)
	{

		ADValue = ReadDataFromAD(AD_CHANNEL_0);
		
		SendTrace(ADValue);
			
	}*/
	
	TURN_ANGLE TurnAngle;

	while(1)
	{

		if (CheckForEnoughSpace(OD_SENSOR_FRONT) == 0)
		{

			StopRobot();

			if ((TurnAngle = FindEnoughSpace(OD_SENSOR_FRONT)) != TURN_ANGLE_MAX)
			{

				TurnRobot(TurnAngle, FORWARD);				

			}
			else if((TurnAngle = FindEnoughSpace(OD_SENSOR_BACK)) != TURN_ANGLE_MAX)
			{
			
				TurnRobot(TurnAngle, BACKWARD);

			}
			else
			{

				if (CheckForEnoughSpace(OD_SENSOR_BACK))
				{		

					DriveRobotBackward();

				}
				else
				{

					StopRobot();

				}
			
			}
		
		}
		else
		{

				
			DriveRobotForward();
			

		}

	}
}

int 
main(void)
{

	InitODRobot();

	StartODRobot();

}
