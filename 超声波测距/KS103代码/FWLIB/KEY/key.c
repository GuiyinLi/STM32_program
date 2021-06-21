#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//KEY0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB4
	
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
}
	
//按键处理函数，返回按键值
//mode=0 不支持连续按，=1支持连续按
//KEY0按下返回1 KEY1按下返回2
//KEY0的优先级大于KEY1
u8 KEY_Scan(u8 mode)
{
	static u8 key_up=1;   //按键松开标志，每次调用该函数时，此语句只执行一次
	if(mode) key_up=1;
	if(key_up&&(KEY0==0||WK_UP==1))
	{
		delay_ms(50);   //延时抖动
		key_up=0;
		if(KEY0==0) return KEY0_PRES;
		else if(WK_UP==1) return WK_UP_PRES;
	}
	else if(KEY0==1&&WK_UP==0)  key_up=1;
	return 0;    //无按键按下 返回1
}

	
			
	


