/*
 *  TraceTest.c
 *
 *  Author: Shanthanand R Kutuva
 */ 


#include <avr/io.h>

#include "Trace.h"

int main(void)
{
	InitTrace(8000000, ERROR_LEVEL | WARNING_LEVEL | INFO_LEVEL);

	SendTrace(ERROR_LEVEL, "This is a character trace message - %c\r\n", 'M');
		
    SendTrace(ERROR_LEVEL, "This is a string trace message - %s\r\n", "Error Trace Message");
		
    SendTrace(ERROR_LEVEL, "This is a signed positive decimal trace message - %d\r\n", 20000);
		
	SendTrace(ERROR_LEVEL, "This is a signed negative decimal trace message - %d\r\n", -20000);
		
	SendTrace(ERROR_LEVEL, "This is a unsigned decimal trace message - %u\r\n", 20000);
		
	SendTrace(ERROR_LEVEL, "This is a 32 bit hex trace message - %lx\r\n", 0xc0000000);
		
	SendTrace(ERROR_LEVEL, "This is a 64 bit hex trace message - %llx\r\n", 0xd000000000000000);
}