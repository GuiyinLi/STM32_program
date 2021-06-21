#include "common.h"
#include "include.h"
#include "sci.h"
#include "isr.h"
#include "imgprocess.h"
#include "globle.h"
#include "motor.h"
#include "servo.h"
#define  T_average 6 //初始化后求赛道中心次数
u8 img_bin_buff[CAMERA_H][CAMERA_W] ; //二值化图像存放
                            //场计数
                //现在所在赛道类型
void main(void)
{
        
	LED_init();        					
        led(LED0,LED_OFF); 
        sci_init(UART4);
       // exti_init(PORTC,10,rising_down);
        Servo_init();
        Motor_init();
        Handle(0);
        while(1)
	{
         FTM_PWM_Duty(FTM1, CH0,450); 
          
          
        }
}