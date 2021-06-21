#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "timer.h"
#include "dma.h"
#include "adc.h"
#include "oled.h"
#include "iic.h"

#define N 10 //取10次平均值
#define M 2 //2路AD

u16 value[N][M]={0};   //存储ADC转换后的M*N个数字量的数据  N和M的位置不能交换！！！
u16 aftervalue[M]={0};   //存储M个通道的ADC值
float avalue[M]={0};     //存储滤波后的M个通道对应的电压值
u8 i=0,j=0;
u16 v1=0,v2=0;

//滤波函数
void filter(void)
{
	int sum=0;
	u8 count;
	for(i=0;i<M;i++)
	{
		for(count=0;count<N;count++)
		{
			sum+=value[count][i];
		}
		aftervalue[i]=sum/N;
		sum=0;
	}
}
int main(void)
{ 
	u16 pwmvalue=0;
	u8 flag=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);		  //初始化延时函数
	uart_init(115200);
	IIC_Init();
	OLED_Init();
	OLED_Display();
	LED_Init();		        //初始化LED端口
	ADC1_Init();
	
	MYDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC1->DR,(u32)&value,M*N);
	MYDMA_Enable(DMA2_Stream0);
	//KEY_Init();
	//EXTIX_Init();
	//TIM2_Int_Init(10000-1,8400-1);  //1s
	//TIM2_PWM_Init(500-1,8400-1);  //分频系数84   84M/84=1MHZ
	while(1)
	{
		/*filter();                        //滤波
		if((v1!=aftervalue[0])||(v2!=aftervalue[1]))
		{
			LED=!LED;
			printf("CH0:%d  CH1:%d\r\n",aftervalue[0],aftervalue[1]);
			printf("CH0:%.1fV  CH1:%.1fV\r\n\r\n",avalue[0],avalue[1]);
			v1=aftervalue[0];
			v2=aftervalue[1];
		}
		for(i=0;i<M;i++)
		{
			avalue[i]=(float)(aftervalue[i]*(3.3/4096));
		}*/
		LED=0;
//		if(!flag) pwmvalue++;
//		else pwmvalue--;
//		if(pwmvalue>300) flag=1;
//		if(pwmvalue==0) flag=0;
//		TIM_SetCompare1(TIM2,pwmvalue);	//修改比较值，修改占空比
	}
}

