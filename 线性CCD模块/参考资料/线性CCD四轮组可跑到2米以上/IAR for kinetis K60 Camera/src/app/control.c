#include "common.h"
#include "include.h"
#include "sci.h"
#include "isr.h"
#include "imgprocess.h"
#include "globle.h"
#include "control.h"
int Nowoutput=0;
int multiple_para =100;
int  sensorErrLastD=0;
int  sensorErrNowD=0;
int sensorErrNow=0 ;
int lastoutput=0;
int  sensorErrLast=0;
int  delta_angle =0;  
int Servo_PD(int Kp,int Kd,int set,int input)
{
     
  
 
  
  sensorErrNow = set-input; //e(k)
    //sent_frame(sensorErrNow,0x04,0xa1,0xaa );
  sensorErrNowD = (s16)(sensorErrNow - sensorErrLast);//delta_e(k)
    //sent_frame(sensorErrNowD,0x04,0xa1,0xaa );
    //sent_frame( sensorErrLastD,0x04,0xa1,0xaa );
  sensorErrLastD= sensorErrNowD - sensorErrLastD;//delta_e(k)-delta_e(k-1)
    
    //sent_frame( sensorErrLastD,0x04,0xa1,0xaa );
  Nowoutput=(Kp*sensorErrNow+Kd*sensorErrLastD)/multiple_para;
  //Nowoutput=(Kp*sensorErrNowD+Kd*sensorErrLastD)/multiple_para;
  /* delta_u(k)={kp*delta_e(k)+ki*e(k)+kd*(delta_e(k)-delta(k-1)}    */
    //sent_frame( Nowangle,0x04,0xa1,0xaa );
   sensorErrLastD=sensorErrNowD;
  lastoutput=Nowoutput;
  sensorErrLast = sensorErrNow;
  return -Nowoutput;
}

void resetpd(void)
{
 sensorErrLastD=0;
 sensorErrNowD=0;
 sensorErrNow=0 ;
 lastoutput=0;
 sensorErrLast=0;
 delta_angle =0; 
}

u8 Motor_PI(s16 *Kp,s16 *Ki,s16 *set,s16 *Nowspeed)
{
}

s16 getangle(s16 row)
{
  
  volatile s16 Ka=1;
  return Ka*row;
}