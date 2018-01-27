#ifndef _UART_TRACE_H_
#define _UART_TRACE_H_

#define BAUD_RATE_9600 51

#define ASCII_CONVERSION	48

#define ASCII_TAB			9

void 
InitUART();

void 
SendTrace(unsigned int Value);

void 
Transmit(unsigned char Value);

#endif
