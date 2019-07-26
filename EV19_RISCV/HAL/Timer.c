#include "Timer.h"


void delay(uint32_t ticks)
{
	while(ticks--)
		asm("nop");
}



