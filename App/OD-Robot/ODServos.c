#define F_CPU 1000000UL

#include <avr/io.h>

#include <avr/interrupt.h>

#include "ODServos.h"

#include "PowerGating.h"

#include "UART-Trace.h"

#include <util/delay.h>

unsigned char Dummy;

void 
InitODServos()
{
	//
	// Set PB0, PB1 to output mode.
	//	

	DDRB |=	(1 << PB0);

	//
	// Set 8 bit timer to PWM Mode with
	// Prescalar = 256, Top = 0xFF,
	// Set OC0 on compare match when up counting,
	// Clear OC0 on compare match when down counting
	//
	
	TCCR0 = ((1 << WGM00) | (1 << COM00) | (1 << COM01) | (1 << CS02));

	//
	// Enable Timer/Counter0 overflow interrupt
	//

	TIMSK |= (1 << TOIE0);

	//
	// Set Timer/Counter register to zero
	//

	TCNT0 = 0;

	//
	// Set Output Compare Match Register to zero 
	//(this register does not get used in normal mode)
	//

	OCR0 = 0xFF;

	//
	// Place both front and back servos in NEUTRAL position
	//

	TurnODServo(OD_SERVO_FRONT, NEUTRAL);

	TurnODServo(OD_SERVO_BACK, NEUTRAL);

}

void
TurnODServo(OD_SERVO ODServo,
			TURN_ANGLE TurnAngle)
{

	WaitForODServoTurnPulse(PWM_PULSES_1);

	TurnOnOffODServo(ODServo, POWER_ON);

	switch (TurnAngle)
	{
		
		case LEFT_90:
		{
			
			OCR0 = OD_ANTICLOCKWISE_90;
				
			break;
			
		}
		
		case LEFT_45:
		{		

			OCR0 = OD_ANTICLOCKWISE_45;
			
			break;
			
		}

		case NEUTRAL:
		{

			OCR0 = OD_NEUTRAL;
			
			break;		

		}		

		case RIGHT_45:
		{
		
			OCR0 = OD_CLOCKWISE_45;
			
			break;
			
		}
		
		case RIGHT_90:
		{		

			OCR0 = OD_CLOCKWISE_90;
			
			break;
			
		}
		
		default:
		{

			goto Exit;
			
		}			
	
	}

	//
	// Delay for 10 seconds
	//

	_delay_ms(10000);

Exit:

	TurnOnOffODServo(ODServo, POWER_OFF);

}

void
WaitForODServoTurnPulse(unsigned int PulseCount)
{

	unsigned int ODServoPWMPulseCount = 0;

	sei();

	while (ODServoPWMPulseCount < PulseCount)
	{
		while (1)
		{
			if (TIFR & (1 << TOV0))
			{

				ODServoPWMPulseCount++;

				break;
			}
		}
	}

	cli();

}

ISR(TIMER0_OVF_vect)
{

	Dummy++;
	
}

