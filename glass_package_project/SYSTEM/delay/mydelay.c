#include "mydelay.h"

void my_delay_ms(u32 ms)
{
	 u16 i=0;
	while(ms--)
	{
		i=4050;
		while(i--);
	}
}
void my_delay_us(u32 us)
{
	u16 i=0;
	while(us--)
	{
		i=2;
		while(i--);
	}
}