/******************** 唐野岐士 ********************
 * 文件名       ：angle.c

 *
 * 实验平台     ：LQ K60开发板
 *                ang PTB11  Gyto PTB10  Z PTE24
 *
 * 作者          DAQI
**********************************************************************************/

#include "include.h"
uint8 angle[15000];
uint8 Speed[2];
uint8 ccd[32];
/*******************    角度计算相关参数**********************/

uint16  Gyro_Offest;        //陀螺仪中间值
uint16  Gravity_Offest;
float Voltage_Gyro;     //AD采集陀螺仪数值
float Voltage_Gravity;  //AD采集陀螺仪数值
float GravityAngle;     //加速度计计算出的角度值，单位  角度
float GyroAngleSpeed;   //陀螺仪计算出  角速度值，单位 角度/毫秒
float NowCarAngle;      //角度估计值，用于卡尔曼滤波
float RealCarAngle;     //角度最佳值，用于卡尔曼滤波
float NowCarAngle_PP;   //估计值方差，用于卡尔曼滤波
float RealCarAngle_PP;   //最佳值方差，用于卡尔曼滤波
float GyroAngle_QQ;     //陀螺仪方差，用于卡尔曼滤波
float GravityAngle_RR;  //加速度计方差，用于卡尔曼滤波
float Kg;               //卡尔曼比例系数，用于卡尔曼滤波
float Car_Angle_Set ;   //倾角设定值，由速度设定值与速度实际值差值确定

/****************      电机控制相关参数   *******************/

float AngleControlOut;    //倾角控制输出
float K_AngleControlOut;   //直立控制系数
float AngleControl_P;    //倾角控制参数，回复系数
float AngleControl_D;    //倾角控制参数，阻尼系数
uint16 Encoder_Right;
uint16 Encoder_Left;
float Speed_Right;        //编码器  右轮车速，周期20毫秒，未加任何修正系数
float Speed_Left;         //编码器  左轮车速，周期20毫秒，未加任何修正系数
float Speed_Set;          //车模速度给定值，同编码器速度相对应 一般为CCD采集回参数处理后加一基数
float Car_Speed;          //车模实际速度，左、右编码器速度相加的一半
uint8 Speed_Flag;
uint8 SpeedFlag;
float Speed_S;
float Speed_X;
float Speed_Control_P;    //速度控制参数，比例系数
float Speed_Control_I;    //速度控制参数，积分系数

//float Speed_Control_D;

float SpeedControlIntegral;//速度积分数,位移
float SpeedControlOutNew;  
float SpeedControlOutOld;
float SpeedControlOut;     //速度控制输出
float K_SpeedControlOut;      //速度控制输出
float  DirectionControlOutNew;  
float  DirectionControlOutOld;   
float  DirectionControlOut;      //方向控制输出
float  Dir_Control_P;            //方向控制参数
uint8 SpeedControlPeriod;  //速度控制周期，每毫秒钟加一，使输出光滑
float MotorControlOut_Right;//右轮电机输出，为各种参数相加后结果，直接送到电机
float MotorControlOut_Left; //左轮电机输出，为各种参数相加后结果，直接送到电机
float Speed_Delt;
//float Speed_Delt_Old;
uint16 time_flag;
/****************      线性CCD相关参数   *******************/

uint16 Loction[120];         //存放线性CCD数值，前四个和后四个数据不太准确，已删除
float Integration_Set;       //曝光量设定值
float Integration_Real;      //曝光量实际值
uint16 Integration_Middle;   //数据中间阀值，加权之后
uint8  Line_left;            //左侧黑线位置，范围0-64
uint8  Line_right;            //右侧黑线位置，范围0-64
float  Integration_time;    //曝光时间 0—20毫秒
float  LeftRightAdd;        //左右黑线位置之差，用于调节转弯和速度设定
float  LeftRightSub;        //左右黑线位置之和，用于调节转弯和速度设定

uint8 barrier;

float GyroAngle;   
uint16 flag;
uint16 flag_w;
uint16 angle_max;
uint16 angle_min;

void Kalman_Filter(void)                    //卡尔曼滤波函数  
{
  Voltage_Gyro=Gyro_value();
  Voltage_Gravity=Gravity_value();
  
  Voltage_Gyro=Gyro_Offest-Voltage_Gyro;
  Voltage_Gravity-=Gravity_Offest;
  if(Voltage_Gyro>1700)
  {
    Voltage_Gyro=1700;    
  }
  if(Voltage_Gyro<-1700)
  {
    Voltage_Gyro=-1700;
  }                                   //限值处理，防止陀螺仪的数据过大错误
  GyroAngleSpeed=Voltage_Gyro*Gyro_Ratio;
  GyroAngle+=GyroAngleSpeed;
//  flag=GyroAngle;
  GravityAngle=Voltage_Gravity*Gravity_Ratio;
 // GravityAngle_RR=((GravityAngle-RealCarAngle)*24+2)*((GravityAngle-RealCarAngle)*24+2);
  NowCarAngle=RealCarAngle+GyroAngleSpeed;                   //估计值
  NowCarAngle_PP=RealCarAngle_PP+GyroAngle_QQ;              //估计方差
  Kg=sqrt(NowCarAngle_PP/(NowCarAngle_PP+GravityAngle_RR)); //求出比例系数
  RealCarAngle=NowCarAngle+Kg*(GravityAngle-NowCarAngle);    //最优值估计
  RealCarAngle_PP=(1-Kg)*NowCarAngle_PP;                     //最优值方差

}
void AngleControl(void)                 //直立控制，每毫秒一次
{
 AngleControlOut=(Car_Angle_Set-RealCarAngle)*AngleControl_P+(0-GyroAngleSpeed)*AngleControl_D; 
}

void MotorSpeedOut(void)              //电机输出，每毫秒一次     
{
//  MotorControlOut_Left=AngleControlOut*K_AngleControlOut+SpeedControlOut*K_SpeedControlOut+DirectionControlOut;
// MotorControlOut_Right=AngleControlOut*K_AngleControlOut+SpeedControlOut*K_SpeedControlOut-DirectionControlOut;
  MotorControlOut_Left=AngleControlOut*K_AngleControlOut+SpeedControlOut*K_SpeedControlOut+DirectionControlOut;
  MotorControlOut_Right=AngleControlOut*K_AngleControlOut+SpeedControlOut*K_SpeedControlOut-DirectionControlOut;
//  MotorControlOut_Right*=1.08;
  if(RealCarAngle>60)
  {
    MotorControlOut_Left=0;
    MotorControlOut_Right=0;
  }
  if(RealCarAngle<-60)
  {
    MotorControlOut_Left=0;
    MotorControlOut_Right=0;
  }
  if(MotorControlOut_Right>=0)
  {
    MotorControlOut_Right+=MOTOR_OUT_DEAD_VAL;
    if(MotorControlOut_Right>150)
    MotorControlOut_Right=150;
    R_PWM_BACK(MotorControlOut_Right);
    R_PWM_GO(0);
  }
  if(MotorControlOut_Right<0)
  {
    MotorControlOut_Right-=MOTOR_OUT_DEAD_VAL;
    if(MotorControlOut_Right<-150)
    MotorControlOut_Right=-150;
    R_PWM_GO((0-MotorControlOut_Right));
    R_PWM_BACK(0);
  }
  if(MotorControlOut_Left>=0)
  {
    MotorControlOut_Left+=MOTOR_OUT_DEAD_VAL;
    
    if(MotorControlOut_Left>150)
    MotorControlOut_Left=150;
    L_PWM_BACK(MotorControlOut_Left);
    L_PWM_GO(0);
  }
  if(MotorControlOut_Left<0)
  {
    MotorControlOut_Left-=MOTOR_OUT_DEAD_VAL;
    if(MotorControlOut_Left<-150)
    MotorControlOut_Left=-150;
    L_PWM_GO((0-MotorControlOut_Left));
    L_PWM_BACK(0);
  }
}
void Get_Speed(void)                //编码器返回实时速度，并计算出控制量
{
  float fP,fI;
  Encoder_Right=FTM1_CNT;
  FTM1_CNT=0;
  Encoder_Left=FTM2_CNT;
  FTM2_CNT=0;
  if(Encoder_Right>30000)
  {
    Speed_Right=(Encoder_Right-65535);
  }
  if(Encoder_Right<=30000)
  {
    Speed_Right=Encoder_Right;
  }
  if(Encoder_Left>30000)
  {
    Speed_Left=(65535-Encoder_Left);
  }
  if(Encoder_Left<=30000)
  {
    Speed_Left=0-Encoder_Left;
  }
  Car_Speed=(Speed_Right+Speed_Left)*0.5;
//  if(Car_Speed<0)
//      Speed[1]=0-Car_Speed;
//  else
//  {
//      Speed[1]=Car_Speed;
//   }
//  Speed[0]=Speed_Set;
  
//  if(NRF24L01_TxPacket(Speed)==TX_OK)
//        {
//          LED_turn(LED3);
//        }
//  Speed_Delt_Old=Speed_Delt;
  Speed_Delt=Speed_Set-Car_Speed;
  if(Speed_Delt>0)
   Speed_Flag=0; 
  if((Speed_Delt<=0)&&(Speed_Set>45))
  {
    Speed_Flag++;
    if((Speed_Delt>=-8)&&(Speed_Flag>=15))
        asm("nop"); 
    else
        SpeedControlIntegral-=0.64;
    if(SpeedControlIntegral<=10)
        SpeedControlIntegral=10;
  }
  if(Speed_Delt<-15)
    Speed_Delt=-15;
  
    
  fP=Speed_Delt*Speed_Control_P;
  if((Speed_Delt<30)&&(Speed_Delt>-30))
  {
  fI=Speed_Delt*Speed_Control_I;
  SpeedControlIntegral+=fI;
  }
  if(Speed_Delt>10)
  {
    SpeedControlIntegral+=0.4;
  }
//  fD=(Speed_Delt-Speed_Delt_Old)*Speed_Control_D;
  SpeedControlOutOld=SpeedControlOutNew;
  SpeedControlOutNew=fP+SpeedControlIntegral;
}
void SpeedControlOutput(void)         
{
  float Value;
  Value=SpeedControlOutNew-SpeedControlOutOld;
  if(Value>SpeedOutLimit)
      Value=SpeedOutLimit;
  if(Value<-SpeedOutLimit)
      Value=-SpeedOutLimit;            //速度输出控制限值处理，防止速度突变太大，摔倒
 // SpeedControlOut=Value*(SpeedControlPeriod+1)/Speed_Control_Period+SpeedControlOutOld;
  SpeedControlOut=Value*0.5+SpeedControlOutOld;
  
}

void CCD_Init(void)
{
  gpio_init(PORTC,18,GPO,0);
  gpio_init(PORTC,19,GPO,0);
}
void StartIntegration(void)   //曝光函数,输出复位序列
{
  uint8 i;
  TSL_SI_High;
  delayus();
  TSL_CLK_High;
  delayus();
  TSL_SI_Low;
  delayus();
  SamplingDelay();
  TSL_CLK_Low;
  delayus();
  for(i=1;i<128;i++)
  {
    TSL_CLK_High;
    SamplingDelay();  
    TSL_CLK_Low;
    delayus();
  }
  TSL_CLK_High;
  delayus();
  TSL_CLK_Low;
}

void Get_Loction(void)       //得到小车位置，即120个数据点，估计耗时七百微秒。
{  
  
  uint8 i;
//  Integration_sum=0;
  TSL_SI_High;
  delayus();
  TSL_CLK_High;
  delayus();
  TSL_SI_Low;
  delayus();
 // Loction[0]=CCD_value();
// Integration_sum+=CCD_value();
  TSL_CLK_Low;
  delayus();
  for(i=0;i<4;i++)
  {
    TSL_CLK_High;
    SamplingDelay(); 
    TSL_CLK_Low;
    delayus();
  }
  for(i=0;i<120;i++)
  {
    TSL_CLK_High;
    delayus();
    Loction[i]=CCD_value();
 //   Integration_sum+=CCD_value();
    TSL_CLK_Low;
    delayus();
  }
  for(i=0;i<4;i++)
  {
    TSL_CLK_High;
    SamplingDelay(); 
    TSL_CLK_Low;
    delayus();
  }
//  Integration_Real=Integration_sum/128;
  
  TSL_CLK_High;
  delayus();
  TSL_CLK_Low;
  
}
void Loction_Filter(void)     //小车位置处理：提取左右侧黑线、计算所需曝光时间、计算出相应方向控制输出
{
  uint8 i,j,k;
  uint32 Integration_sum_Black,Integration_sum_White;
  uint32 Integration_sum;
  j=0;
  k=0;
  Integration_sum_Black=0;
  Integration_sum_White=0;
  Integration_sum=0;
  for(i=0;i<120;i++)
  {
    Integration_sum+=Loction[i];
  }
  Integration_Real=Integration_sum/120;    //计算出实际曝光量
  Integration_time+=(Integration_Set-Integration_Real)*0.0005; //计算出所需曝光时间 
  if(Integration_time>=18)
    Integration_time=18;
  if(Integration_time<=1)
    Integration_time=1;      //曝光时间限制处理 
  
  for(i=0;i<120;i++)
  {
    if(Loction[i]<=Integration_Real-5)
    {
       Integration_sum_Black+=Loction[i];
       j++;
    }
    if(Loction[i]>Integration_Real+5)
    {
       Integration_sum_White+=Loction[i];
       k++;
    }
    Integration_Middle=(Integration_sum_Black/j+Integration_sum_White/k)/2;   //加权得出中间阀值
  }
  if((Loction[58]<Integration_Middle)&&(Loction[59]<Integration_Middle)&&(Loction[60]<Integration_Middle)&&(Loction[61]<Integration_Middle))
  {
    for(i=59;i>=1;i--)
    {
      if((Loction[i]>Integration_Middle)&&(Loction[i-1]>Integration_Middle))
      {
       Line_left=80;
       break;
      }
    }
    for(j=60;j<=119;j++)
    {
      if((Loction[j]>Integration_Middle)&&(Loction[j+1]>Integration_Middle))
      {
      Line_right=80;
      break;
      }
    }
    if((i<=40)&&(j>=80))
       barrier=1;
  }  //如果中间四个数据点为黑线，接下来开始提取白线，白线在哪一方，说明车应向该方向转
  if(!((Loction[58]<Integration_Middle)&&(Loction[59]<Integration_Middle)&&(Loction[60]<Integration_Middle)&&(Loction[61]<Integration_Middle)))
  {    
    for(i=59;i>=1;i--)
    {
  
       if((Loction[i]<Integration_Middle)&&(Loction[i-1]<Integration_Middle))
       {
        Line_left=59-i;
        break;
        }
    }    
   if(i<=1)
     Line_left=60;
  for(i=60;i<=118;i++)
  {
    if((Loction[i]<Integration_Middle)&&(Loction[i+1]<Integration_Middle))
    {
      Line_right=i-60;
      break;
    }
  }
  if(i>=118)
     Line_right=60;
  }
  //提取左右黑线
  DirectionControlOutOld=DirectionControlOutNew;
  
  LeftRightAdd= Line_left+Line_right;
  LeftRightSub= Line_left-Line_right;
  DirectionControlOutNew=LeftRightSub*Dir_Control_P/LeftRightAdd; 
  
  
  
}
void DirectionControlOutput(void)
{
  float Value;
  Value=DirectionControlOutNew-DirectionControlOutOld;
  if(Value>DirectionOutLimit)
    Value=DirectionOutLimit;
  if(Value<-DirectionOutLimit)
    Value=-DirectionOutLimit;
  DirectionControlOut=Value*(SpeedControlPeriod+1)/Speed_Control_Period+DirectionControlOutOld;
}
void CarControl(void)
{
    Kalman_Filter();
    AngleControl();
    SpeedControlOutput();
    DirectionControlOutput();
    SpeedControlPeriod++;
    
    if(SpeedControlPeriod==(19-(uint8)Integration_time))
    StartIntegration();
    if(SpeedControlPeriod==19)
    Get_Loction();
    if(SpeedControlPeriod==20)
    {  
      Loction_Filter();
      
      Get_Speed();
      
      SpeedControlPeriod=0;
    }
    MotorSpeedOut();
//  }
 
  
  
/*  if(SpeedControlPeriod==1)
    convery_ccd0();
  if(SpeedControlPeriod==5)
    convery_ccd1();
  if(SpeedControlPeriod==9)
    convery_ccd2();
  if(SpeedControlPeriod==13)
    convery_ccd3();*/

}
  
void convery_ccd0(void)
{
  uint8 i;
  for(i=0;i<4;i++)
    ccd[i]=0;
  for(i=4;i<32;i++)
  {
    ccd[i]=Loction[i-4]/6;
  }
  if(NRF24L01_TxPacket(ccd)==TX_OK)
  LED_turn(LED0);
}
void convery_ccd1(void)
{
  uint8 i;
  for(i=0;i<32;i++)
  {
    ccd[i]=Loction[i+28]/6;
  }
  if(NRF24L01_TxPacket(ccd)==TX_OK)
  LED_turn(LED1);
}
void convery_ccd2(void)
{
  uint8 i;
  for(i=0;i<32;i++)
  {
    ccd[i]=Loction[i+60]/6;
  }
  if(NRF24L01_TxPacket(ccd)==TX_OK)
  LED_turn(LED2);
}
void convery_ccd3(void)
{
  uint8 i;
  for(i=0;i<28;i++)
  {
    ccd[i]=Loction[i+92]/6;
  }
  for(i=0;i<4;i++)
    ccd[i+28]=0;
  if(NRF24L01_TxPacket(ccd)==TX_OK)
  LED_turn(LED3); 
}
