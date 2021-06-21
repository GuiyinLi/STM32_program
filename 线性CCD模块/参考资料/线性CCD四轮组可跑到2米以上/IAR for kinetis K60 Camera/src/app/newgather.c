

#include "common.h"
#include "include.h"
#include "sci.h"
#include "DMA.h"
u8 img_bin_buff[CAMERA_H][CAMERA_W]; //图像存放位置


void main(void)
{
        unsigned int  i,j;
	         					//初始化，设置背景为白色
	Ov7725_Init((u8 *)img_bin_buff);          	//摄像头初始化
        sci_init(UART4);
       // LED_init();
        for(i=0;i<CAMERA_H;i++)
  {
           for(j=0;j<CAMERA_W;j++)
    {
      
          img_bin_buff[i][j]=j;
      
     
     }
      
  }
  sent_image(img_bin_buff);
          while(1)
	{
	    //led(LED0,LED_ON);
                   img_flag = IMG_START;					//开始采集图像
                    PORTA_ISFR=~0;							//写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
                    enable_irq(87); 						//允许PTA的中断
                while(img_flag != IMG_FINISH);
           
            //led(LED0,LED_OFF);
            
	    sent_image(img_bin_buff);
            PORTA_ISFR=~0;	
          // enable_irq(87); 
		
		
	}
}
