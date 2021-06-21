#include "motor.h"

short int vlset,vrset;
short int vlnow,vrnow;
short int vlold,vrold;

int16 VS=0;


int16 Vset,err_1,err_2,Vnow,PWM;

int16 pwml,pwmr;   //当前设定的占空比

void init_motor()
{
    
  FTM_PWM_init(FTM0,CH0,10000,0);//PTD4
  FTM_PWM_init(FTM0,CH5,10000,0);//PTD5
  FTM_PWM_init(FTM0,CH1,10000,0);//PTD4
  FTM_PWM_init(FTM0,CH6,10000,0);//PTD5
  vlset=0;vrset=0;
  pwml=0;pwmr=0;
  err_1=0;err_2=0;PWM=0;
}

void setSpeed(uint16 l,uint16 r)
{
  vlset=l;vrset=r;
}


void setSpeedPWM(int16 m,int16 n)
{
  if(m>10000)m=10000;
  if(m<0)m=0;
  if(n>10000)n=10000;
  if(n<0)n=0;
  FTM_PWM_Duty(FTM0,CH1,m);
  FTM_PWM_Duty(FTM0,CH6,n);
}

void speedCorr_PID()
{
    const float P= 21.8888888888 ,//2.888888
                //I= 0.085 ,
                I= 3.0,//-0.8666666666,
                D= 9.0 ;//0.15
    int16 err,PWMcorr;
    int16 ee=(int16)Vset/50;
    err=Vset-Vnow;
    if(((err>0)&&(err-ee>0))||((err<0)&&(err+ee<0)))
    {
        PWMcorr=(int16)(P*(err-err_1)+I*err+D*(err-2*err_1+err_2));
        PWM+=PWMcorr;
    }
    err_2=err_1;
    err_1=err;
    if(PWM>10000)PWM=10000;if(PWM<1)PWM=0;
    setSpeedPWM(PWM,PWM);
}
 