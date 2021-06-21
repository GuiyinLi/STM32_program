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



#include "common.h"
#include "include.h"
#include "DMA.h"
#include "SCI.h"
#include <stdbool.h>
volatile u16 Vnum=0;
void USART4False_IRQHandler()
{
  uart_putchar(UART4,0xbb);
  uart_putchar(UART4,0xbb);
}

/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：HardFault_Handler
*  功能说明：硬件上访中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-4    已测试
*  备    注：可以用LED闪烁来指示发生了硬件上访
*************************************************************************/
void HardFault_Handler(void)
{
    while (1)
    {
        printf("\n****硬件上访错误!!!*****\r\n\n");
    }
}



void USART4_IRQHandler(void)
{
	
	static int i;
	bool  frame_flag=0;
	unsigned char sci_temp;
        DisableInterrupts;
	sci_temp=uart_getchar(UART4);
	 if(frame_flag)
		{
			sci_indata[i++]=sci_temp;
		}
		if(sci_temp==0x7b)//7b={
		{
			frame_flag=1;
			i=0;
		}
		if(sci_temp==0x7d)//7d=}
		{
			frame_flag=0;
			i=0;
			sci_inprocess();
		}
         EnableInterrupts;
}

/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：PORTA_IRQHandler
*  功能说明：PORTA端口中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-1-25    已测试
*  备    注：引脚号需要自己初始化来清除
*************************************************************************/
void PORTA_IRQHandler()
{
    u8  n = 0;    //引脚号
	
	//static u8  imagerow=0; //所需要的行数，最大CAMERA_H9
        static u16 row=0; //记录现在的行数
    /*==============================================================================
	注意：
		场中断放在行中断前面，即优先级高于行中断。
		场中断用户任务里，全部都return，确保不进入行中断的用户任务。
	
	==============================================================================*/
    n = 27;											//场中断
    if(PORTA_ISFR & (1 << n))           			//PTA27触发中断
    {
        
        /*  以下为用户任务  */
		
		//场中断需要判断是场结束还是场开始
		if(img_flag == IMG_START)					//需要开始采集图像
		{
			DMA_DADDR(CAMERA_DMA_CH)= (u32)IMG_BUFF;//马上恢复目的地址
			DMA_EN(CAMERA_DMA_CH);            		//使能通道CHn 硬件请求      (这样才能继续触发DMA传输)
                      
		}
        PORTA_ISFR  = ~0;        			        //场中断里，全部都要清中断标志位
		return;										//场中断触发了，就不需要处理行中断
		
        /*  以上为用户任务  */
    }
	
    //==============================================================================
    if(img_flag != IMG_GATHER)						//只有图像在采集中，才处理行中断
	{
                PORTA_ISFR  = ~0;  
		return;
	}

    
    n = 26;
    if(PORTA_ISFR & (1 << n))                       //PTA26触发行中断
    {
        PORTA_ISFR  = ~0;        			//写1清中断标志位
        /*  以下为用户任务  */
        
#if CAMERA_DMA_ERROR
			DMA_DADDR(CAMERA_DMA_CH)  +=  CAMERA_DMA_ERROR ;		//马上恢复目的地址
#endif
	if(row<=CAMERA_H)//row==data_table[imagerow])//如果当前行数据应该采集
         {
	    DMA_EN(CAMERA_DMA_CH); //使能通道CHn 硬件请求      (这样才能继续触发DMA传输)
            //imagerow++;
         } 
        else
        {
          DMA_DIS(CAMERA_DMA_CH);            		//禁止通道硬件DMA请求
                       
			disable_irq(87); 						//关闭PTA的中断
                        img_flag = IMG_FINISH;//标记图像采集完毕
                        row=0;
        }
     
        row++;
		
       /*  以上为用户任务  */
    }
    
    PORTA_ISFR  = ~0;        			//写1清中断标志位
}



/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：PIT0_IRQHandler
*  功能说明：PIT0 定时中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-18    已测试
*  备    注：
*************************************************************************/
void PIT0_IRQHandler(void)
{
        sent_frame(1000,0x06,0xa1,0xaa);
	Site_type site={10,70};
	//disable_irq(87); 							//关闭PTA的中断
	LCD_Num_8x16_C(site,Vnum,BLUE,RED);		//显示实际采集的行数
	Vnum=0;
	//enable_irq(87); 							//允许PTA的中断
    PIT_Flag_Clear(PIT0);       				//清中断标志位
	
}

