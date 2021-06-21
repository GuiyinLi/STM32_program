#include "ccd.h"
#include "adc.h"	
#include "usart.h"
#include "string.h"
#include "math.h"
#include "delay.h"
#include "dma.h"
u16 ccd_adc;
ccdData_Type ccd_piexl[128]={0};
ccdData_Type Median,Threshold;   //线性CCD  相关
ccdData_Type Last_Median;
ccdData_Type Left_Side,Right_Side;

//CCD初始化
void CCD_Init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //PA.1 端口配置 SI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.1
	GPIO_SetBits(GPIOA,GPIO_Pin_2);						 //PA.1 输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	    		 //PA.3 端口配置, 推挽输出  CLK
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	GPIO_SetBits(GPIOA,GPIO_Pin_3); 						 //PA.3	输出高
	
	#if DMA_Tran //采用DMA传输
	ADC1_init();
	MYDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)&ccd_adc,10); 
	
	#else
	ADC_init();
	#endif
	
}


//延时函数，控制曝光时间 *
void Dly_us(int a)
{
   int ii;    
   for(ii=0;ii<a;ii++);      
}


//按照时序依次读取CCD输出的模拟电压值 *
void RD_TSL(void) 
{
	u8 i=0,tslp=0;
  TSL_CLK=1;
  TSL_SI=0; 		//产生第129个CLK以结束本次采样
  Dealy;
	
  TSL_SI=1; 
  TSL_CLK=0;
	Dealy;
	
  TSL_CLK=1;
  TSL_SI=0;     //使SI产生一个下降沿 AO开始采集模拟数据
	Dealy;
	
  for(i=0;i<128;i++)//在每个CLK下降沿时采集一个像素 共采集128个像素
  { 
    TSL_CLK=0; 			//CLK拉低 每次下降沿采集一次数据
		Dealy;
		#if DMA_Tran //采用DMA传输
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		ccd_piexl[tslp++]=(ccdData_Type)ccd_adc*0.0625;
		ADC_SoftwareStartConvCmd(ADC1, DISABLE);
		#else
    ccd_piexl[tslp++]=(ccdData_Type)(Get_Adc(1))*0.0625;   //(Get_Adc(1))>>4
		#endif
    TSL_CLK=1;
		Dealy;
  }
}

//线性CCD取中值
void getCCDMedian(void)
{ 
	 static u16 i,j;
	 static ccdData_Type Left,Right;
	 static ccdData_Type value1_max,value1_min;
	
	 value1_max=ccd_piexl[0];  //动态阈值算法，读取最大和最小值
   for(i=5;i<123;i++)   //两边各去掉5个点
   {
      if(value1_max<=ccd_piexl[i])
				value1_max=ccd_piexl[i];
   }
	 value1_min=ccd_piexl[0];  //最小值
   for(i=5;i<123;i++) 
   {
      if(value1_min>=ccd_piexl[i])
				value1_min=ccd_piexl[i];
   }
   Threshold=(value1_max+value1_min)/2;	  //计算出本次中线提取的阈值
	 for(i = 5;i<118; i++)   //寻找左边跳变沿
	 {
			if(ccd_piexl[i]>Threshold&&ccd_piexl[i+1]>Threshold&&ccd_piexl[i+2]>Threshold&&ccd_piexl[i+3]<Threshold&&ccd_piexl[i+4]<Threshold&&ccd_piexl[i+5]<Threshold)
			{	//去除3个以下像素点的毛刺
				Left=i;
				Left_Side=Left;
				break;	
			}
	 }
	 for(j = 118;j>5; j--)//寻找右边跳变沿
   {    
			if(ccd_piexl[j]<Threshold&&ccd_piexl[j+1]<Threshold&&ccd_piexl[j+2]<Threshold&&ccd_piexl[j+3]>Threshold&&ccd_piexl[j+4]>Threshold&&ccd_piexl[j+5]>Threshold)
			{	//去除3个以下像素点的毛刺
				Right=j;
				Right_Side=Right;
				break;
			}
   }
	Median=(Right+Left)/2;//计算中线位置
	if(fabs(Median-Last_Median)>70)   //计算中线的偏差，如果太大
		Median=Last_Median;    //则取上一次的值
	Last_Median=Median;  //保存上一次的偏差
}


