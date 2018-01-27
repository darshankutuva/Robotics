#ifndef _SEVEN_SEGMENT_DISPLAY_H_
#define _SEVEN_SEGMENT_DISPLAY_H_

#include <avr/io.h>

#define MAX_NUMBERS 10

#define ZERO 	((1 << PA6) | (1 << PA5) | (1 << PA4) | (1 << PA3) | (1 << PA2) | (1 << PA0))

#define ONE 	((1 << PA5) | (1 << PA4))

#define TWO 	((1 << PA6) | (1 << PA5) | (1 << PA1) | (1 << PA2) | (1 << PA3))

#define THREE 	((1 << PA6) | (1 << PA5) | (1 << PA1) | (1 << PA4) | (1 << PA3))

#define FOUR 	((1 << PA0) | (1 << PA1) | (1 << PA4)) | (1 << PB5)

#define FIVE 	((1 << PA6) | (1 << PA0) | (1 << PA1) | (1 << PA4) | (1 << PA3))

#define SIX 	((1 << PA0) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA1))

#define SEVEN 	((1 << PA6) | (1 << PA5) | (1 << PA4))

#define EIGHT 	((1 << PA6) | (1 << PA5) | (1 << PA4) | (1 << PA3) | (1 << PA2) | (1 << PA1) | (1 << PA0))

#define NINE 	((1 << PA6) | (1 << PA5) | (1 << PA1) | (1 << PA0) | (1 << PA4))



void 
InitSevenSegmentDisplay();

void 
DisplayValue(unsigned int Value);

#endif
