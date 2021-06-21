#include "adc.h"
#include "delay.h"

//初始化ADC
//开启ADC1的通道0~7,对应关系如下
//ADC1_CH0-->PA0	
//ADC1_CH1-->PA1	
//ADC1_CH2-->PA2
//ADC1_CH3-->PA3	
//ADC1_CH4-->PA4	
//ADC1_CH5-->PA5	
//ADC1_CH6-->PA6
//ADC1_CH7-->PA7
void ADC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef  ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);  //使能IO口和ADC时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;               //模拟输入引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;    //PA0-1
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;       //悬空
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	 ADC_DeInit();                        //将ADC设为缺省值 此句不能省去 不然无法实现多路ADC
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
	
	//初始化ADC
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_12b;  //12为数据
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;            //扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;      //连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;   //软件触发
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;     //数据右对齐
	ADC_InitStructure.ADC_NbrOfConversion=2;               ///两个转换通道
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_1;  //开启DMA
	ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;      //独立模式
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
	ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;  //两次采样时间间隔为5个周期
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_480Cycles);  //480个周期
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_480Cycles);
	
	
	ADC_Cmd(ADC1,ENABLE);                                //使能ADC1
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE); //源数据变化时 开启DMA传输
	ADC_DMACmd(ADC1,ENABLE);                          //使能ADC的DMA通道
	ADC_SoftwareStartConv(ADC1);                //软件打开ADC
	
}


