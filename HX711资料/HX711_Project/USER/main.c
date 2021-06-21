#include "stm32f10x.h" 
#include "sys.h" 
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "hx711.h"
#include "key.h"

 int main(void)
{
	delay_init();
	LED_Init();
	OLED_GPIO_Init();
  OLED_Init();
	KEY_Init(); 
	HX711_Init();
	delay_ms(1000);  //等待hx711模块稳定
	Get_Gross_Weight();
	Init_Gross_Weight=Gross_Weight;
	Gross_Weight=0;
	OLED_Display();
	while(1)
	{
		Get_Net_Weight();
		OLED_ShowNum(32,2,Net_Weight,4,16);  //显示净重
		OLED_ShowNum(32,6,All_Weight,4,16);  //显示净重
		if(Flag_Error)
		{
			led=0;   //指示灯亮表明超重
		}
		else
		{
			led=1;
		}
		if(KEY0==0)  //去除毛重
		{
			delay_ms(50);
			if(KEY0==0)
			{
			  Get_Gross_Weight();   //去皮
				OLED_P8x16Str(0,4,"Del Gross");
			}              
		}
		else if(KEY1==0)  //算上毛重
		{
			delay_ms(50);
			if(KEY1==0)
			{
				Gross_Weight=0;
				OLED_P8x16Str(0,4,"Inc Gross");
			}
		}
	}
}


