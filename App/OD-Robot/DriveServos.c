#define F_CPU 1000000UL

#include <avr/io.h>

#include <avr/interrupt.h>

#include "DriveServos.h"

#include "UART-Trace.h"

#include <util/delay.h>

void 
InitDriveServos()
{
	//
	// Set pin PD5 (OC1A) to output.
	//
	
	DDRD |= (1 << PD5);

	//
	// Set pin PE2 (OC1B) to output.
	//

	DDRE |= (1 << PE2);

	//
	// Set TCNT1 to zero.
	//
	
	TCNT1 = 0;

	//
	// Set values for TCCR1A
	// Clear OC1A on compare match when up counting.
	// Set OC1A on compare match when down counting.
	// Top = 0xFF
	//

	TCCR1A = ((1 << WGM10) | (1 << COM1A1) | (1 << COM1B1) | (1 << COM1A0) | (1 << COM1B0));

	//
	// Set values for TCCR1B
	// PWM, Phase and Frequency Correct, PRE SCALER to 256.
	//

	TCCR1B = (1 << CS12);

	//
	// Enable TOIE1 interrupt
	//

	TIMSK |= (1 << TOIE1);

	//
	// Set OCR1A Value
	//	

	OCR1A = 0xFF;
	
	//
	// Set OCR1B Value
	//
	
	OCR1B = 0xFF;
		
}

void
DriveRobotForward()
{

	//
	// Wait until we reach the end of a Clock Pulse
	//

	WaitForRobotTurnPulse(PWM_PULSES_1);

	OCR1A = DS_CLOCKWISE_90;

	OCR1B = DS_ANTICLOCKWISE_90;

}

void
DriveRobotBackward()
{

	//
	// Wait until we reach the end of a Clock Pulse
	//

	WaitForRobotTurnPulse(PWM_PULSES_1);

	OCR1A = DS_ANTICLOCKWISE_90;

	OCR1B = DS_CLOCKWISE_90;

}

void
StopRobot()
{

	OCR1A = 0xFF;

	OCR1B = 0xFF;

}

void 
TurnRobot(TURN_ANGLE TurnAngle,
		  TURN_DIRECTION TurnDirection)
{

	//
	// First Stop the Robot
	//	

	StopRobot();	

	//
	// Wait until we reach the end of a Clock Pulse
	//

	WaitForRobotTurnPulse(PWM_PULSES_1);

	if (TurnDirection == FORWARD)
	{
		
		switch (TurnAngle)
		{
	
			case LEFT_90:

			case LEFT_45:
			{
			
				OCR1A = DS_CLOCKWISE_90;
			
				break;
				
			}
		
			case RIGHT_45:
				
			case RIGHT_90:
			{

				OCR1B = DS_ANTICLOCKWISE_90;
			
				break;			

			}

			default:
			{

				break;

			}
		}

	}
	else
	{

		switch (TurnAngle)
		{

			case LEFT_90:

			case LEFT_45:
			{
			
				
				OCR1A = DS_ANTICLOCKWISE_90;

				break;
				
			}

		
			case RIGHT_90:

			case RIGHT_45:
			{

			
				OCR1B = DS_CLOCKWISE_90;

				break;
				
			}

			default:
			{

				break;

			}

		}

	}

	//
	// Now wait until we satisfy the required number of pulses
	//
	
	switch (TurnAngle)
	{

		case RIGHT_90:

		case LEFT_90:
		{

			WaitForRobotTurnPulse(PWM_PULSES_24);

			break;

		}


		case RIGHT_45:

		case LEFT_45:
		{

			WaitForRobotTurnPulse(PWM_PULSES_11);

			break;

		}

		default:
		{

			break;

		}

	}

	StopRobot();

}

void
WaitForRobotTurnPulse(unsigned int PulseCount)
{

	unsigned int RobotPWMPulseCount = 0;

	sei();

	while (RobotPWMPulseCount < PulseCount)
	{
		while (1)
		{
			if (TIFR & (1 << TOV1))
			{

				RobotPWMPulseCount++;

				break;
			}
		}
	}

	cli();

}

ISR(TIMER1_OVF_vect)
{

}


