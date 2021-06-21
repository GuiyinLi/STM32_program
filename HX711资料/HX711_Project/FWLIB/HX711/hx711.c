#include "hx711.h"
#include "delay.h"
#define  filter_times 2
long Init_Gross_Weight=0;  //初始皮重
long All_Weight=0;         //总重量
long Gross_Weight = 0;
long Net_Weight = 0;
u8 Flag_Error=0;

/* ----------------------------- */
/* HX711 延时函数                */
/* ----------------------------- */
void Hx711_Delay()
{
	__NOP();         //一个机器周期
}
/* ----------------------------- */
/* HX711 SCLK DOUT引脚初始化     */
/* DOUT PB6                      */
/* SCLK PB7                      */
/* ----------------------------- */
void HX711_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//SCLK
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//DOUT
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/* ----------------------------- */
/* 读取HX711                     */
/* 128增益                       */
/* ----------------------------- */
long HX711_Read()
{
  long value;
	char i;
	DOUT_High;  //数据线拉高
	Hx711_Delay();
	SCLK_Low;   //数据线拉低
	value=0;
	while(DOUT);     //等待拉低
	for(i=0;i<24;i++)
	{
		SCLK_High;
		value<<=1;
		SCLK_Low;
		if(DOUT)  //判断是否拉高
			value++;
	}
	SCLK_High;
	value=value^0x800000;//第25个脉冲下降沿来时，转换数据
	Hx711_Delay();
	SCLK_Low;
	return value;
}

/* ----------------------------- */
/* 获取毛皮重量                  */
/* ----------------------------- */
void Get_Gross_Weight()
{
	  Gross_Weight=HX711_Read();
	  Gross_Weight-=Init_Gross_Weight;
}

/* ----------------------------- */
/* 获取实际重量                  */
/* ----------------------------- */
void Get_Net_Weight()
{
	All_Weight=filter();  //防止负数突变
	Net_Weight=All_Weight-Gross_Weight;
	if(All_Weight>=0)  //计算实际重量  净重大于零 总重肯定大于零
	{
		Net_Weight = (unsigned int)((float)Net_Weight/GapValue); 	//计算实物的实际重量 单位是g
		All_Weight= (unsigned int)((float)All_Weight/GapValue);
		if(All_Weight>4900)  //提前报警
			Flag_Error=1;
		else
			Flag_Error=0;
	}
	else    //不用进行报警 偶尔会出现负数
	{
		Flag_Error=1;
		Net_Weight=0;
		All_Weight=0;
	}
}

/* 限幅平均滤波 */
long filter()
{
	long  last_value=0;  //上次采集值
	long  new_value=0;   //当前采集值
	long  sum=0;
	char count;
	for(count=0;count<filter_times;count++)
  {
		new_value=HX711_Read()-Init_Gross_Weight; 
		if(new_value<0&&last_value>=0) 
			new_value=last_value; 
		sum+=new_value; 
      	last_value=new_value;
  }
  return (long)((float)sum/(float)filter_times); 
}
	





