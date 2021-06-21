/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：exti.c
 * 描述         ：EXTI外部GPIO中断例程
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：野火嵌入式开发工作室
 * 淘宝店       ：http://firestm32.taobao.com
 * 技术支持论坛 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/

#include "include.h"



/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：exti_init
*  功能说明：EXTI外部GPIO中断初始化
*  参数说明：PORTx       端口号（PORTA,PORTB,PORTC,PORTD,PORTE）
*            n           端口引脚
*            exti_cfg    触发选项和上拉下拉选项
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
uint8 k;
void  exti_init(PORTx portx, u8 n, exti_cfg cfg)
{
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK << portx);    //开启PORTx端口

    PORT_PCR_REG(PORTX[portx], n) = PORT_PCR_MUX(1) | PORT_PCR_IRQC(cfg & 0x7f ) | PORT_PCR_PE_MASK | ((cfg & 0x80 ) >> 7); // 复用GPIO , 确定触发模式 ,开启上拉或下拉电阻
    GPIO_PDDR_REG(GPIOx[portx]) &= ~(1 << n);       //输入模式
    enable_irq(portx + 87);                         //使能PORT中断，PORTA的ISR中断号为87
}


/*************************************************************************
*                             唐野岐士
*                              freescale光电平衡组
*  函数名称：key_init
*  功能说明：按键初始化
*  KEY1 PTC8   KEY2  PTC14
*  函数返回：无
*  修改时间：2013-4-2
*  备    注：
*************************************************************************/
void key_init(void)
{
  PORT_PCR_REG(PORTX[PORTC], 6) = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK ; // GPIO功能 , 内部上拉
  PORT_PCR_REG(PORTX[PORTC], 14) = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK ; // GPIO功能 , 内部上拉
  GPIO_PDDR_REG(GPIOx[PORTC]) &= ~(1 <<6);    //  确定引脚为输入
  GPIO_PDDR_REG(GPIOx[PORTC]) &= ~(1 << 14);    // 确定引脚为输入
}
/*************************************************************************
*                             唐野岐士
*                              freescale光电平衡组
*  函数名称：key1  key2
*  功能说明：按键扫描
*  KEY1 PTC8   KEY2  PTC14
*  函数返回：无
*  修改时间：2013-4-2
*  备    注：
*************************************************************************/
void key1(void)
{ 
  if(GPIO_Get(PORTC,6)==0)
    delayms(30);
  if(GPIO_Get(PORTC,6)==0)
  {
    while(!(GPIO_Get(PORTC,6)));
          k=1;
  }
}
void key2(void)
{ 
  if(GPIO_Get(PORTC,14)==0)
    delayms(30);
  if(GPIO_Get(PORTC,14)==0)
  {
    while(!(GPIO_Get(PORTC,14)));
          LED_turn(LED3);
  }
} 
