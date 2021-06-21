#include "stm32f10x.h"
#include "sys.h"
#include "led.h"
#include "delay.h"     //引用延时函数头文件
int main()
{
	LED_Init();        //LED初始化
	delay_init();     //延时函数初始化
	while(1)
	{
		/*************位操作法********************/
		/*LED0=1;
		LED1=0;
		delay_ms(500);      //延时500ms
		LED1=1;
		LED0=0;
		delay_ms(500);*/
		/***************************库函数法*********************/
		GPIO_SetBits(GPIOB,GPIO_Pin_5);                   //将PB5置位高电平
		GPIO_ResetBits(GPIOE,GPIO_Pin_5);                   //将PE5置位高电平
		delay_ms(500);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);                   //将PB5置位高电平
		GPIO_SetBits(GPIOE,GPIO_Pin_5);                   //将PE5置位高电平
		delay_ms(500);
	}
}


