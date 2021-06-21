#include "stm32f10x.h" 
#include "sys.h" 
#include "delay.h"
#include "led.h"
#include "key.h"
#include "spi.h"
#include "oled.h"
#include "rc522.h"
#include "timer.h"
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();
	LED_Init();
	OLED_GPIO_Init();
	OLED_Init();
	KEY_Init();
	while(RC522_Init());   //等待初始化成功
	OLED_Display();
	TIM2_Int_Init(1999,7199);     //200ms   控制灯
	while(1)
	{
		RFID_Handle(2);  //读取扇区2
	}
		
	
}






