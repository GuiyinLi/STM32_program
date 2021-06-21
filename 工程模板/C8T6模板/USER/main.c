#include "stm32f10x.h" 
#include "sys.h" 
#include "delay.h"
#include "led.h"
int main(void)
{
	LED_Init();
	delay_init();
	while(1)
	{
		led=!led;
		delay_ms(1000);
	}
}


