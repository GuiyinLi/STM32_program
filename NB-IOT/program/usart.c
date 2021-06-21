#include "usart3.h"
#include "delay.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x_tim.h"
//串口发送缓存区
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节
                                                  //__align()用于修改最高级别对象的字节边界 来保证数据对象是相应对齐
#ifdef USART3_RX_EN                               //如果使能了接收
//串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //接收缓冲,最大USART3_MAX_RECV_LEN个字节.

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART3_RX_STA = 0;
void USART3_IRQHandler(void)
{
    u8 res;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
    {
        res = USART_ReceiveData(USART3);
        if (USART3_RX_STA < USART3_MAX_RECV_LEN) //还能接收数据
        {
            TIM_SetCounter(TIM3, 0); //清空计数器
            if (USART3_RX_STA == 0)
                TIM3_Set(1);                    //使能接收中断
            USART3_RX_BUF[USART3_RX_STA] = res; //记录接收到的值
            USART3_RX_STA++;
        }
        else
            USART3_RX_STA |= 1 << 15; //超出最大字节 强制接收完成
    }
}
//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率
void USART3_Init(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    USART_DeInit(USART3); //复位串口3

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //PB10 TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PB11 RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;                                     //一般为9600
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式
    USART_Init(USART3, &USART_InitStructure);

    USART_Cmd(USART3, ENABLE); //使能串口3
#ifdef USART3_RX_EN            //如果使能了接收

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //开启中断

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //相应优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM3_Init(99, 7199); //50ms中断
    USART3_RX_STA = 0;   //清零
    TIM3_Set(0);         //关闭定时器3
#endif
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //产生更新中断
    {
        USART3_RX_STA |= 1 << 15;                   //标记接收完成
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除中断标志位
        TIM3_Set(0);                                //关闭定时器3
    }
}
//设置TIM3的开关
//sta:0，关闭;1,开启;
void TIM3_Set(u8 sta)
{
    if (sta)
    {
        TIM_SetCounter(TIM3, 0); //计数器清空
        TIM_Cmd(TIM3, ENABLE);   //使能TIMx
    }
    else
        TIM_Cmd(TIM3, DISABLE); //关闭定时器4
}

//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
void TIM3_Init(u16 arr, u16 psc)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能//TIM3时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //根据指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能指定的TIM3中断,允许更新中断

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化VIC寄存器
}
#endif
