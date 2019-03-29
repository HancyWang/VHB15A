#include <INTRINS.H>
#include "delay.h"

void delay_us(u16_t t)
{
	while(--t)
	{
	   _nop_();
	}
}


void delay_ms(u16_t t)
{
	unsigned int i;
	while(--t)
	{
		 i=1100;
		 while(--i)
	   {
	      _nop_();
	   }
	}
}

