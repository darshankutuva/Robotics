#define F_CPU 1000000UL

#include <util/delay.h>

#include "SevenSegmentDisplay.h"

void 
InitSevenSegmentDisplay()
{

	DDRA = ((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6));

}

void 
DisplayValue(unsigned int Value)
{
	unsigned short Digit;

	while (Value != 0)
	{

		Digit = Value % 10;

		Value = Value / 10;

		switch (Digit)
		{
			case 0:
			{
				
				PORTA = ZERO;
				
				break;
						
			}
			case 1:
			{

				PORTA = ONE;

				break;
				
			}
			case 2:
			{

				PORTA = TWO;

				break;

			}
			case 3:
			{

				PORTA = THREE;

				break;

			}
			case 4:
			{

				PORTA = FOUR;

				break;

			}
			case 5:
			{

				PORTA = FIVE;

				break;

			}
			case 6:
			{

				PORTA = SIX;

				break;

			}
			case 7:
			{

				PORTA = SEVEN;

				break;

			}
			case 8:
			{

				PORTA = EIGHT;

				break;

			}
			case 9:
			{

				PORTA = NINE;

				break;

			}

		}
		
		_delay_ms(10000);

	}
}
