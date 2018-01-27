#include <avr/io.h>

#include "common.h"

//
// Enumeration
//

typedef enum _MESSAGE_TYPE
{
	ACK_MESSAGE		= 0,
	
	GENERAL_MESSAGE	= 1
	
} MESSAGE_TYPE;

bool
InitXymk5vXyfst(unsigned char MyAddress, unsigned char DestAddress, bool EnableAutoReTransmit);

bool
Xymk5vXyfstTransmit(PVOID Buffer, unsigned long Length, MESSAGE_TYPE MessageType);

bool
Xymk5vXyfstReceive(PVOID Buffer, unsigned long *Length, MESSAGE_TYPE MessageType);