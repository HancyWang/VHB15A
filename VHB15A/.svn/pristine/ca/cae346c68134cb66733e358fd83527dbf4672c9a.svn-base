#include <INTRINS.H>
#include "delay.h"
#include "data_type.h"
void delay_us(u16_t t)
{
	while(--t>(u16_t)0)
	{
	   _nop_();
	}
}


void delay_ms(u16_t t)
{
	uint16_t i;
	while(--t>(u16_t)0)
	{
		i=1100;
		while(--i>(u16_t)0)
		{
			_nop_();
		}
	}
}

