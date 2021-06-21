/**************************************************************************
 ** 创建日期：  2012-11-2                                                 *
 ** 文件名  ：  isr.c                                                     *
 ** 版本    ：	                                                          *
 ** 平台    ：  羽良工作室MK60DN512ZVLQ10最小系统板                       *
 ** 函数列表：	                                                          *
 ** 功能描述：  OV7620摄像头数据采集                                      *
 ** 创建者  ：  羽良工作室 yullion.taobao.com                                                *
 ** 修改历史：                                                            *
 ** @版权所有： 羽良工作室 yullion.taobao.com                                                *
 ** 淘宝店：    yullion.taobao.com                                        *
 **************************************************************************/

/*******************************************************************************************
欢迎光临淘宝店  羽良工作室  yullion.taobao.com
K60淘宝链接     http://item.taobao.com/item.htm?spm=a1z10.1.w4.18.rMwvaU&id=21039172293
********************************************************************************************/

#include "common.h"
#include "include.h"
#include "isr.h"
#include "dma.h"
#include "calculation.h"

u8 TIME0flag_5ms  = 0 ;
u8 TIME0flag_10ms = 0 ;
u8 TIME0flag_15ms = 0 ;
u8 TIME0flag_20ms = 0 ;
u8 TIME0flag_80ms = 0 ; 
u8 TIME1flag_20ms = 0 ;
u8 TIME1flag_1ms  = 0 ;
u8  TimeCount = 0 ;
int zhilitime_count=0  ;
u8 zhili_flag=1;

u8 TIME0flag_d1ms  = 0 ;
u8 TIME0flag_d2ms  = 0 ;
u8 TIME0flag_d3ms  = 0 ;
u8 TIME0flag_d4ms = 0 ;
u8 TIME0flag_d5ms = 0 ;
//u8 TIME0flag_20ms = 0 ;

int countl;
int countr;

extern int     g_nSpeedControlPeriod;
extern int g_nDirectionControlPeriod;
extern int dier_count_period;

volatile u8  pit_flag = 0;
volatile u32 dma_int_count = 0;

/******************************************************************************/
unsigned char Pix_Data[V][H] = {0};                                     //采集Ｖ行 Ｈ列的图像数据
unsigned char V_Cnt=0;                                                  //行采集计数
unsigned char Is_SendPhoto=0;                                           //图像发送标志
unsigned char FieldCounter=1;                                           //判断奇偶场
/******************************************************************************/
#ifdef TIAOSHI    //调试模式
//串口调试时执行的程序段，发送出所有的图像数据
/*************************************************************************
*  函数名称：DMA_CH4_Handler
*  功能说明：DMA通道4的中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：
*  备    注：
*************************************************************************/

void DMA_CH4_Handler(void)
{
//DMA_IRQ_CLEAN(DMA_CH4);                                 //清除通道传输中断标志位    (这样才能再次进入中断)
 //DMA_DIS(DMA_CH4);                                       //采集完H个数据后进入这个DMA中断，停止DMA传输。行中断中打开DMA传输

    //串口调试用到
    //if(V_Cnt == 160)
     // Is_SendPhoto = 1;
}
   
/************************************************************************/
//晓克添加
//行中断处理函数
void PORTA_IRQHandler()
{
   extern u8 Camera_Flag;
   u8 n;
       n = 10;
    if(PORTA_ISFR & (1 << n))         //PTA10触发中断
    {
        PORTA_ISFR  |= (1 << n);        //写1清中断标志位
        countl++;
    }
    n = 12;
    if(PORTA_ISFR & (1 << n))         //PTA11触发中断
    {
        PORTA_ISFR  |= (1 << n);        //写1清中断标志位
        countr++;
    }
 
      
}

//场中断处理函数
void PORTB_IRQHandler()
{
  extern u8 Camera_Flag;
if(PORTB_ISFR & (1<<0))                                 //PTB0触发中断,场中断
    {
      
        PORTB_ISFR  = ~0;  
      if(Camera_Flag==1)
      {
        DMA_DADDR(DMA_CH4)=(u32)Pix_Data;
          DMA_EN(DMA_CH4);
          
        PORTA_ISFR  = ~0; 
         Camera_Flag=0;
      
      //DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Pix_Data, PTC0, DMA_BYTE1,H, DMA_rising_keepon); 
        //DMA通道4初始化，PTC0上升沿触发DMA传输，源地址为PTD_BYTE0_IN，目的地址为：Pix_Data ，每次传输1Byte，传输H次后停止传输，目的地址保持不变，关闭通道CHn 硬件请求
        enable_irq(87);   //使能行中断				//标记图像采集中
       disable_irq(88);  
         V_Cnt=0;                                           //行采集计数清零
      }
      
      else if (Camera_Flag== 0)
         {
           disable_irq(87);
           disable_irq(88);
           Camera_Flag=0;
         }
     //偶场 
    }
      
      
  
      
      
      
      /************************************************************************/
    }


#endif










/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：PIT0_IRQHandler
*  功能说明：PIT0 定时中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-18    已测试
*  备    注：
*************************************************************************/


extern int zhijiao_2ms_nb;
int zhijiao_count;
void PIT0_IRQHandler(void)
{
  static u8 TimeCount ; 
  static unsigned char TimerCnt20ms = 0;
  PIT_Flag_Clear(PIT0);       //清中断标志位
  extern u8 IntegrationTime ;             //曝光时间
  extern void StartIntegration(void);   
  if(zhijiao_2ms_nb)
  {
    zhijiao_count++;   
  }
 // static unsigned char TimerCnt20ms = 0;
  u8 integration_piont;

  TIME1flag_1ms = 1 ;
   
  TimerCnt20ms++;

 //IntegrationTime=19;
  integration_piont = 10 - IntegrationTime; 
  if(integration_piont >= 2) {      /* 曝光时间小于2则不进行再曝光 */
      if(integration_piont == TimerCnt20ms)

      StartIntegration();//曝光开始
 

  /* 根据曝光时间计算20ms周期内的曝光点*/
 
        

  }
  if(TimerCnt20ms >= 20) {
    TimerCnt20ms = 0;
    TIME1flag_20ms = 1;
  }
  
  
  
  
  
  TimeCount ++ ;
  if(zhili_flag==1)
  {
  zhilitime_count++;
  }
  //TimerCnt20ms++ ; 


  
  
  
  if(TimeCount>= 5 )
   {
 
     TIME0flag_d5ms = 1;

     // if(g_nDirectionControlPeriod>=10)
     //  g_nDirectionControlPeriod=0;
   
     TimeCount = 0;

 } 
  
   
   
   else if(TimeCount ==1 ){
     
     TIME0flag_d1ms = 1;
     
     

 } 
   
   
   
   
   else if(TimeCount ==2 ){
     
     //g_nDirectionControlPeriod=g_nDirectionControlPeriod+5;
     //Direct_output();
     
     TIME0flag_d2ms = 1;

  } 
   else if(TimeCount ==3 ){
     TIME0flag_d3ms = 1;
     
  }
  else if(TimeCount ==4 ){
     TIME0flag_d4ms = 1;
  }
  


   
}

void FTM0_IRQHandler()
{
  
  extern u16 bianmaqi_count;
uint8 Status;
Status=FTM0_STATUS;
FTM1_STATUS=0x00;//清中断标志位
bianmaqi_count++;
}



void DMA_CH0_Handler(void)
{

    //DMA通道0
    printf("DMA_CH0_Handler\n");
    DMA_IRQ_CLEAN(DMA_CH0);                             //清除通道传输中断标志位    (这样才能再次进入中断)
    DMA_EN(DMA_CH0);                                    //使能通道CHn 硬件请求      (这样才能继续触发DMA传输)

}

/*************************************************************************
*                             蓝宙嵌入式开发工作室
*
*  函数名称：PIT1_IRQHandler
*  功能说明：PIT1定时中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-9-18    已测试
*  备    注：
*************************************************************************/
void PIT1_IRQHandler(void)
{
   PIT_Flag_Clear(PIT1);       //清中断标志位
    disable_irq(69);            //PIT1
    disable_irq(87);            //PORTA
    extern short int vlnow;
    extern short int vrnow;
    vlnow=(short int)(countl);
    vrnow=(short int)(countr);
    extern int16 Vnow;
    Vnow=(vlnow+vrnow)>>1;
    countl=0;countr=0;
 //   E6A2_debug_PrinttoScreen();
 //   bt_debug_speed();
    speedCorr_PID();
    enable_irq(87);             //PORTA
    enable_irq(69);             //PIT1
  

}



extern float DIR_CONTROL_P;
extern float DIR_CONTROL_D;

u8 Rx_Count=0;
u8 Rx_Buff[20];
void USART2_IRQHandler()
{
  u8 i=0;u8 tmp;
  float j = 1.0;float temp=0;
  extern int16 VS;
  uart_irq_DIS(UART2);
 // DIR_CONTROL_P=
  tmp = uart_getchar(UART2);
  if(tmp != '/')
  {
    Rx_Buff[Rx_Count] = tmp;
    Rx_Count++;
  }else
  {
    Rx_Buff[Rx_Count] = '/';
    
    /*START*/
    i=0;
    while(!((Rx_Buff[i]>='0')&&(Rx_Buff[i]<='9')))i++;
    temp = Rx_Buff[i] - '0';i++;
    while((Rx_Buff[i]!='/') && (i<10))
    {
      
      if(Rx_Buff[i]=='.')
        j=0.1;
      else
      {
        if(j!=1.0)
        {
          temp = temp + (Rx_Buff[i] - '0') * j;
          j=j*0.1;
        }
        else
          temp = temp * 10 + Rx_Buff[i] - '0';
      }
      i++;
      
    }
    if(Rx_Buff[0]=='P')
      DIR_CONTROL_P = temp * (-1);
    if(Rx_Buff[0]=='D')
      DIR_CONTROL_D = temp *(-1);
    if(Rx_Buff[0]=='V')
      VS = (int16)temp;
    
    
    /*END*/
    Rx_Count = 0;
    uart_sendStr(UART2, Rx_Buff);
    memset(Rx_Buff, 0, sizeof(Rx_Buff));
    printf("%lf\r\n",0.5);
  }
  

uart_irq_EN(UART2);
}