#include "common.h"
#include "include.h"
#include "sci.h"
#include "isr.h"
#include "imgprocess.h"
#include "globle.h"
#include "servo.h"

u16 abs(s16 value);





void Servo_init(void)
{
  FTM_PWM_init(FTM1, CH0, 250, servo_center); //舵机PWM周期20ms
  
}

void Handle(s16 angle)
{
  s16 now_center;
  
  /*if(angle>=0)
  {
    
     if(angle<=3)
   {
     angle=12;
    }
   else if(angle<10&& angle>3)
   {
     angle=28;
    }
     else if(angle<15&& angle>10)
   {
     angle=39;
    }
 
   else 
   {
     
     angle=2*(angle-1)+26;
    }


  }
  
  
  
  
  else   ///右偏左转（大）
  {
      if(angle>=-3)
   {
     angle=-10;
    }
   else if(angle>-10&& angle<-3)
   {
     angle=-18;
    }
     else if(angle>-15&& angle<-10)
   {
     angle=-25;
    }
 
   else
   {
     angle=2*(angle+9)-26;
    }
  }
  now_center=(u32)servo_center+angle;
  //sent_frame(now_center,0x01,0xa1,0xaa );*/
  if(angle<10 && angle>=0)
  {
    angle=0;
  }
  else if(angle>-10 && angle<=0)
  {
    angle=0;
  }
  
 
  
  angle=servo_center-angle;
  if(angle>650)
  {
    angle+=20;
  }
  
  if(angle)
 
  /*
  if(now_center>730)
  {
    now_center=730;
  }
  else  if(now_center<470)
  {
    now_center=470;
  }  
  FTM_PWM_Duty(FTM1, CH0,now_center);*/
  
  if(angle>750)
  {
    angle=750;
  }
  else  if(angle<470)
  {
    angle=470;
  }
  FTM_PWM_Duty(FTM1, CH0,angle);

}

u16 abs(s16 value)
{
  if(value<0)
  {
    value=-value;
  }
  return (u16)value;
}