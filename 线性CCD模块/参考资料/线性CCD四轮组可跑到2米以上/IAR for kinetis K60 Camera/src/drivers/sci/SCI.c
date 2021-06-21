/********************************** RP  ********************************************
 * File name       ：SCI.c
 * Description     ：Functions related to host computer
* Author           : RP
 

**********************************************************************************/

#include "common.h"
#include "include.h"
#include "SCI.h"
#include "isr.h"
extern int zongjianbianliang;
u8 sci_indata[6];//接收到一帧的数据量 6位={两位标志位+四位数据位}

#define DIR_POS_L0 2
#define DIR_POS_L1 5//20
#define DIR_POS_L2 15//20
#define Pix_Threshold 120
//#define Dirction_angledot_offset 0

extern int Dirction_angledot_offset ;
extern s16  AAngleAccele[6] ;
extern int Direction_M_Old;
extern int Direction_M_New;
extern float  g_fDirectionControlOut;
extern float g_fSpeedControlOut;
extern int g_nDirectionControlPeriod;

extern s16 sci_cmd_data[40][3]={{65,48,600},{65,49,80},{65,50,80},{65,51,80},{65,52,40},{65,53,10},{65,54,0},{65,55,200},{65,56,800},{65,57}
                               ,{66,48,12},{66,49,2000},{66,50},{66,51},{66,52},{66,53},{66,54},{66,55},{66,56},{66,57}
			       ,{67,48},{67,49},{67,50},{67,51},{67,52},{67,53},{67,54},{67,55},{67,56},{67,57}
			       ,{68,48},{68,49},{68,50},{68,51},{68,52},{68,53},{68,54},{68,55},{68,56},{68,57}};//65=A 66=B 67=C 48=0  57=9	
void sci_inprocess(void);
int  acsii_to_value();
/*************************************************************************
*                             RP
*
*  Fuctionname：void sci_init(uartn)
*  Fuction：上位机全图像采集串口4初始化
*  Argument:None
*  Value：None
* 
*************************************************************************/
void sci_init(UARTn uartn)
{
  uart_irq_DIS(uartn);
  uart_init(uartn,115200);//set UART4
  uart_irq_EN(uartn); //Enable UART4 Receive Interruption

}

void sci_vscope(u8 ch0,u8 ch1,u8 ch2,u8 ch3,u8 ch4)
{
  uart_putchar(UART4,0xff);
  uart_putchar(UART4,0xff);
  uart_putchar(UART4,ch0);
  uart_putchar(UART4,ch1);
  uart_putchar(UART4,ch2);
  uart_putchar(UART4,ch3);
  uart_putchar(UART4,ch4);
}

/****************************************
Fuctionname:int sent_image(u8 image_buff[WIDTH][HEIGHT])
Fuction:发送一帧图像到上位机
Value：NONE
****************************************/
void sent_image( )
{
  
}

void sent_image1(void)
{/*
int Black_L0=1;
int Black_R0=160;
int Black_L1=1;
int Black_R1=160;
int Black_L2=1;
int Black_R2=160;


  unsigned char j = 0;
int Track_central ;
int  Position_error;
int error_last_time;
int Direction_angledot;
float Kp_dir=0.0385;//0.034
float Kd_dir=0.00225;//0.00225;//0.00223
float GYRIO=0;

  
 
  
  for(j = 80; j >1; j =j-1)
  {
    if((Pix_Data[DIR_POS_L0][j] < Pix_Threshold) && (Pix_Data[DIR_POS_L0][j - 1] < Pix_Threshold) && (Pix_Data[DIR_POS_L0][j - 2] < Pix_Threshold))
    {
      //if(Line_Black_block_Info[i].Black_L == 0)
      {
        Black_L0 = j;//记录第一次黑块位置
        break;
      }
    }
  }
  
  for(j = 81; j < 160; j =j+1)
  {
    if((Pix_Data[DIR_POS_L0][j] < Pix_Threshold) && (Pix_Data[DIR_POS_L0][j + 1] < Pix_Threshold) && (Pix_Data[DIR_POS_L0][j + 2] < Pix_Threshold))
    {
      //if(Line_Black_block_Info[i].Black_L == 0)
      {
        Black_R0 = j;//记录第一次黑块位置
        break;
      }
    }
  }
 
  Track_central = (unsigned char)((Black_L0 + Black_R0)/2.0);
  */ 
/*if(Track_central >= 236)
  {
    Track_central = 236;
  }
  if(Track_central <= 3)
  {
     Track_central = 3;
  }*/
  //uart_putchar (UART2, Track_central);
  //Position_error=(float)(80 - Track_central);//位置偏差校准

 /* if(Pix_Data[DIR_POS_L0][80]<Pix_Threshold)
    //if(Pix_Data[DIR_POS_L0][80]>=0 && Pix_Data[DIR_POS_L0][80]<=5 )
     Position_error=error_last_time;
  else
     Position_error=(float)(80 - Track_central);//位置偏差校准
  
  error_last_time=Position_error;
  zongjianbianliang=Position_error;
  Direction_angledot = Dirction_angledot_offset - AAngleAccele[2];
  
   //Direction_angledot*=GYRIO;
  
  
   Direction_M_Old=Direction_M_New;
   Direction_M_New = Kp_dir* Position_error + Kd_dir*Direction_angledot;
   //zongjianbianliang=Direction_M_New;
*/
/*u8 c;
u8 r;
 uart_putchar(UART2,0x00);
  uart_putchar(UART2,0xff);
  uart_putchar(UART2,0x01);
  uart_putchar(UART2,0x00);
 
  for(c=0;c<CAMERA_H;c++)
  {
     for(r=0;r<CAMERA_W;r++)
     {
      
      uart_putchar(UART2,Pix_Data[c][r]);
     }
    
  }
*/
}

/*==================================The Programs below belong to Serial Hunter =====================================================*/
/****************************************
Fuctionname:void USART4_IRQHandler()
Fuction:UART4 Interruption Service Routine
Value：NONE
****************************************/
void Direct_output()
{
 /* float fValue; 
  fValue = Direction_M_New - Direction_M_Old;
   //g_fDirectionControlOut = fValue * (g_nDirectionControlPeriod + 1.0) / 10.0 + Direction_M_Old;
   g_fDirectionControlOut = fValue * (g_nDirectionControlPeriod+1 ) / 5.0 + Direction_M_Old;
    
*/
}






/****************************************
Fuctionname:sci_inprocess(void)
Fuction:assign every frame to the matrix
Value：NONE
****************************************/
void sci_inprocess(void)

{
	s16 scivalue=0;
	s16 j;
	scivalue =acsii_to_value();
	
		
	if( sci_indata[0]=='A')
	{
		
		for(j=0;j<10;j++)
		{	
			   
			if((int)sci_cmd_data[j][1]==(int)sci_indata[1])
				{
					sci_cmd_data[j][2]=scivalue;
					
				    j=0;	
					break;
				}
						
		}
	}		
	if(sci_indata[0]=='B')
			{
		for(j=0;j<10;j++)
				{
			if((int)sci_cmd_data[10+j][1]==(int)sci_indata[1])
					sci_cmd_data[10+j][2]=scivalue;
					
					j=0;
					
					break;
				}
			}
	if(sci_indata[0]=='C')
		{
		for(j=0;j<10;j++)
			{
			if((int)sci_cmd_data[20+j][1]==(int)sci_indata[1])
					sci_cmd_data[20+j][2]=scivalue;
					
					j=0;
					
					break;
			}
		}
	if(sci_indata[0]=='D')
		{
			for(j=0;j<10;j++)
			{
				if((int)sci_cmd_data[30+j][1]==(int)sci_indata[1])
					sci_cmd_data[30+j][2]=scivalue;
					
					j=0;
					
					break;
			}
		}
	if(sci_indata[0]=='E')
		{
			for(j=0;j<10;j++)
			{
				if((int)sci_cmd_data[40+j][1]==(int)sci_indata[1])
					sci_cmd_data[40+j][2]=scivalue;
					
					j=0;
					
					break;
			}
		}
}

/**************************************
Fuctionname:acsii_to_value(void)
Fuction:Transfer ASCII to demical number
Value：value in demical  OK!!!!!!
****************************************/
int acsii_to_value(void)
{
	s16 i;
	s16 j=0;//temp for negtive number
	s16 temp[sci_datalenth];//数据位数
    int value=0;
	for(i=0;i<sci_datalenth;i++)
	  temp[i]=(int)sci_indata[i+2];
	  //for(i=0;i<sci_datalenth;i++)
			  //uart0_sentbit(temp[i]);
	for(i=0;i<sci_datalenth;i++)
	 	{temp[i]=temp[i]-48;
		if(temp[i]==-3)
			{
				temp[i]=0;
				j=1;//正负数标志
			}
		}
		value=1000*temp[0]+100*temp[1]+10*temp[2]+temp[3];
		if(j==1)
		value=-value;
		j=0;
                //uart_putchar(UART4,value);
		return value;
		
}

/************************************************************************************************
Fuctionname:void sent_frame(int dec,unsigned char function_code,unsigned char start_flag,unsigned char stop_flag )
Fuction:sent a frame contents 1 integrate num.(2 bytes)
Argument:dec: 16bits value:int
	      functioncode: the function of the frame
	      start flag  : the head/start of this frame
	      stop  flag  : the end of this frame
Value：NONE
**************************************************************************************************/


void sent_frame(s16 dec,u8 function_code,u8 start_flag,u8 stop_flag )

{
	u8 temp[sci_outdatalenth];
	
	temp[2]=dec;
	temp[3]=dec>>8;
	temp[0]=start_flag;
	temp[1]=function_code;
	temp[4]=stop_flag;
	uart_sendN (UART1, temp, sci_outdatalenth);
	
	
}





void SentIMG(void)
{
  
 
}