#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
 #include "LED.h"
 /*
 Keil: MDK5.10.0.2
MCU:stm32f103c8
硬件接法：
GY-53---STM32
1、GY-53_PWM---STM32_PA0
2、STM32_TX---FT232_RX,STM32将数据上传给上位机

软件说明:
该程序采用PWM方式获取模块数据，距离=高电平时间(ms)*100，波特率9600

联系方式：
http://shop62474960.taobao.com/?spm=a230r.7195193.1997079397.2.9qa3Ky&v=1
*/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_X;
  
  /* 4个抢占优先级，4个响应优先级 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /*抢占优先级可打断中断级别低的中断*/
	/*响应优先级按等级执行*/
	NVIC_X.NVIC_IRQChannel = USART1_IRQn;//中断向量
  NVIC_X.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
  NVIC_X.NVIC_IRQChannelSubPriority = 0;//响应优先级
  NVIC_X.NVIC_IRQChannelCmd = ENABLE;//使能中断响应
  NVIC_Init(&NVIC_X);
}
void gpio_Input(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
}
int main(void)
{
	uint8_t datas[3]={0};
	uint16_t distance=0;
	float time=0.0f;
	delay_init(72);
	NVIC_Configuration();
	Usart_Int(9600);
	gpio_Input();
	delay_ms(500);//等待模块初始化完成
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));//等待拉低
	while(1)
	{
		while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));//等待拉高开始计时
		start_Time();
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));//等待拉低，停止计时
		Get_Time(&time);//得到以ms为单位的时间
		distance=time*100;//距离长度=pwm时间(ms)*100
		datas[0]=distance>>8;
		datas[1]=distance;
		datas[2]=2;//模块默认模式为 2
		send_3out(datas,3,0x15);//由于pwm方波周期为50ms，距离最长为2M即time最大为20ms，所以有足够时间将数据通过串口发送
		
	}
}
