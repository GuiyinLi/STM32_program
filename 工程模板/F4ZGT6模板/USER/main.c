#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

int main(void)
{ 
	LED_Init();		        //³õÊ¼»¯LED¶Ë¿Ú
	delay_init(168);		  //³õÊ¼»¯ÑÓÊ±º¯Ê
	while(1)
	{
		LED=0;
		delay_ms(500);
		LED=1;
		delay_ms(500);
	}
}

