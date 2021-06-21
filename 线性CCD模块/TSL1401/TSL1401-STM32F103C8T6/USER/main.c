#include "include.h"
//参考网站： https://blog.csdn.net/weixin_42108484/article/details/102815123
int main(void)
{
	LED_Init();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);
	CCD_Init();
	OLED_Init();
	Screen_Init();
	TIM2_Init_ms(5);
	while(1)
	{
		OLED_Show_CCD();
		Debug(Multi_CCD_Data);
	}
}
