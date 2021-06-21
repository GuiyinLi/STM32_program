/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：isr.c
 * 描述         ：中断处理例程
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
*函数名称：PIT0_IRQHandler
*作用用于测定准确延时
*主要用于测定一段程序的执行时间
其中PIT_LDVAL初始值  PIT_CVAL实际值， 数值由PIT_LDVAL逐渐递减   32 位
**************************************************************************/

uint16 time;
uint16 time1,time2;
uint16 ge;
void PIT0_IRQHandler(void)
{
   CarControl();
   PIT_Flag_Clear(PIT0);
}

/*************************************************************************
*函数名称：NRF24L01_IRQHandler
*作用:用于无线模块接收模式下中断读取接收值

**************************************************************************/         
void NRF24L01_IRQHandler(void)
{
  PORTE_ISFR |= (1<<25);
  uint8 sta;
  sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志 
//  if(sta&RX_OK)//接收到数据
//  {
//	NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD,R_tmp_buf,RX_PLOAD_WIDTH);//读取数据
//	NRF24L01_Write_Reg(NRF24L01_FLUSH_RX,0xff);//清除RX FIFO寄存器
//  //      LED_turn(LED3);
//  }
}

