
#include "include.h"

/*************************************************************************
*                             唐野岐士
*                              freescale光电平衡组

*  建立时间：2013-3-29     
*   系统内核频率96M   BUS_clock  48M   Flex_Clock 48M  Flash_Clock 24M
* 


*    LED引脚说明
*    LED0  PTA14  LED1  PTA15   LED2  PTA16   LED0  PTA17

*    5110引脚说明
    CLK   PTA25  DIN  PTA26    DC PTA27  CS PTA28  RST PTA29

*   按键引脚说明         //按键函数写在了 exti.c中，便于以后改成外部中断
    K2 PTB20   K3  PTB21  K4  PTB22  K5  PTB0  K6  PTB6  K7  PTB8
    
*   AT24C02引脚说明  
    SCL PTD4  SDA  PTD6

*   电机pwm输出  PTC1  PTC2  PTC3  PTC4
    
*    nrf24l01引脚说明
    IRQ PTE25  CSN PTE26  MOSI PTD13   CE PTE28  SCK PTD12    MISO PTD14

*   正交解码编码器引脚说明
      FTM2_PHA  PTA10    FTM2_PHB   PTA11  FTM1_QD_PHA  PTA8  FTM1_QD_PHB  PTA9

*   Gyto PTB2    Gravity PTE24

*  线性CCD引脚说明
*  SI PTC18  SCK PTC19  A0 PTB11
*************************************************************************/
extern volatile uint8 angle[15000];

extern volatile uint8 Speed[2];
extern volatile uint8 ccd[32];
extern volatile uint16 angle_max;
extern volatile uint16 angle_min;
//extern volatile uint8 flag;
//extern volatile uint16 n;
//extern volatile uint16 Gyto0_Offest1[1000];
//extern volatile uint16 temp;
//extern volatile float Gyto0_Speed_Ratio;  //陀螺仪0换算比例值，包含角速度、阻尼系数
//extern volatile float o;

/*******************    角度计算相关参数**********************/

extern volatile uint16   Gyro_Offest;     //陀螺仪中间值
extern volatile uint16   Gravity_Offest;  //倾角中间值
extern volatile float Voltage_Gyro;       //AD采集陀螺仪数值   
extern volatile float Voltage_Gravity;    //AD采集陀螺仪数值
extern volatile float GravityAngle;       //加速度计计算出的角度值，单位  度
extern volatile float GyroAngleSpeed;     //陀螺仪计算出  角速度值，单位 度/毫秒
extern volatile float NowCarAngle;        //角度估计值，用于卡尔曼滤波
extern volatile float RealCarAngle;      //角度最佳值，用于卡尔曼滤波
extern volatile float NowCarAngle_PP;    //估计值方差，用于卡尔曼滤波
extern volatile float RealCarAngle_PP;   //最佳值方差，用于卡尔曼滤波
extern volatile float GyroAngle_QQ;     //陀螺仪方差，用于卡尔曼滤波
extern volatile float GravityAngle_RR;   //加速度计方差，用于卡尔曼滤波
extern volatile float Kg;               //卡尔曼比例系数，用于卡尔曼滤波
extern volatile float Car_Angle_Set ;   //倾角设定值，实际设置为零

/****************      电机控制相关参数   *******************/

extern volatile float AngleControlOut;    //倾角控制输出
extern volatile float K_AngleControlOut;   //直立控制系数
extern volatile float AngleControl_P;    //倾角控制参数，回复系数
extern volatile float AngleControl_D;    //倾角控制参数，阻尼系数
extern volatile uint16 Encoder_Right;
extern volatile uint16 Encoder_Left;
extern volatile float Speed_Right;       //编码器  右轮车速，周期20毫秒，未加任何修正系数  
extern volatile float Speed_Left;        //编码器  左轮车速，周期20毫秒，未加任何修正系数
extern volatile float Speed_Set;         //车模速度给定值，同编码器速度相对应 一般为CCD采集回参数处理后加一基数
extern volatile float Car_Speed;         //车模实际速度，左、右编码器速度相加的一半
extern volatile uint8 Speed_Flag;
extern volatile uint8 SpeedFlag;
extern volatile float Speed_S;
extern volatile float Speed_X;
extern volatile float Speed_Control_P;   //速度控制参数，比例系数
extern volatile float Speed_Control_I;   //速度控制参数，积分系数

//extern volatile float Speed_Control_D;

extern volatile float SpeedControlIntegral;//速度积分数,位移
extern volatile float SpeedControlOutNew;  
extern volatile float SpeedControlOutOld;   
extern volatile float SpeedControlOut;      //速度控制输出
extern volatile uint8 SpeedControlPeriod;   //速度控制周期，每秒钟加一，使输出光滑
extern volatile float K_SpeedControlOut;      //速度控制输出
extern volatile float  DirectionControlOutNew;  
extern volatile float  DirectionControlOutOld;   
extern volatile float  DirectionControlOut;      //速度控制输出
extern volatile float  Dir_Control_P;            //方向控制参数

extern volatile float MotorControlOut_Right;//右轮电机输出，为各种参数相加后结果，直接送到电机
extern volatile float MotorControlOut_Left; //左轮电机输出，为各种参数相加后结果，直接送到电机
extern volatile float Speed_Delt;
extern volatile uint16 time_flag;
/****************      线性CCD相关参数   *******************/
extern volatile uint16 Loction[120];       //存放线性CCD数值，前四个和后四个数据不太准确，已删除
extern volatile float Integration_Set;     //曝光量设定值
extern volatile float Integration_Real;    //曝光量实际值
extern volatile float  Integration_time;    //曝光时间 0—20毫秒
extern volatile uint16 Integration_Middle;   //数据中间阀值，加权之后
extern volatile uint8  Line_left;            //左侧黑线位置，范围0-60
extern volatile uint8  Line_right;           //右侧黑线位置，范围0-60
extern volatile float  LeftRightAdd;         //左右黑线位置之差，用于调节转弯和速度设定
extern volatile float  LeftRightSub;         //左右黑线位置之和，用于调节转弯和速度设定

extern volatile uint8 barrier;

//FTM2_CNT 右轮编码器计数
extern volatile uint16 time,time1;
extern volatile float GyroAngle;
extern volatile uint16 ge;
extern volatile uint16 flag;
extern volatile uint16 flag_w;
extern volatile float Speed_Delt_Old;
void Kalman_Parameter_init(void)
{
  RealCarAngle_PP=0.01;
  GyroAngle_QQ=0.0001;
  GravityAngle_RR=100000;
  RealCarAngle=0;
  NowCarAngle=0;
}
void Motor_Parameter_init(void)
{
  time_flag=0;
//  AngleControl_P=9.8;
//  AngleControl_D=90;
  K_AngleControlOut=1;
  K_SpeedControlOut=0;
  AngleControl_P=7.8;
  AngleControl_D=80;
  Car_Angle_Set=0;
  DirectionControlOutNew=0;
  DirectionControlOutOld=0;
  DirectionControlOut=0;
  Integration_Set=1000;
  Integration_time=5;
  SpeedControlIntegral=0;
  Speed_Set=0;
//  Speed_Control_I=0.04;
  Speed_Control_P=0.7;
  Speed_Control_I=0.2;
//  Speed_Control_D=0.1;
  SpeedControlOut=0;
  SpeedControlPeriod=0;
  Dir_Control_P=0;

}
void all_init(void)
{
  uint8 k;
  Speed_Flag=0;
  Speed_S=0;
  Speed_X=0;
  Speed_Delt=0;
  uint32 Gyro_Offest_Sum,Gravity_Offest_Sum;
  Gyro_Offest_Sum=0;
  Gravity_Offest_Sum=0;
  LCD5110_Init();  //5110液晶初始化
  delayms(10);
  key_init();     //按键初始化
  LED_init();
//  AT24CXX_Init();  // AT24C01初始化
  delayms(100);
  adc_init();
  delayms(10);
//  NRF24L01_Init();
//  delayms(100);
//  TX_Mode();
//  delayms(1000);
  for(k=0;k<200;k++)
  {
    Gyro_Offest_Sum+=Gyro_value();
    Gravity_Offest_Sum+=Gravity_value();
  }
  Gyro_Offest=Gyro_Offest_Sum/200;        
  Gravity_Offest=Gravity_Offest_Sum/200; 
  CCD_Init();
  pit_init(PIT0,48000);
  delayms(100);
  led(LED3,LED_ON);
  FTM_PWM_init();
  FTM_QUAD_init();
}
  
void main(void)
{
  ge=0;
  uint16 i,j;
  float Speed_Rights; 
  
    
  flag_w=0;
  flag=0;
  DisableInterrupts;
  Motor_Parameter_init();
  Kalman_Parameter_init();
  all_init();
  EnableInterrupts;

  
  delayms(1000);
  delayms(1000);
  K_AngleControlOut=0.95;
  K_SpeedControlOut=0.1;
  delayms(100);
  K_AngleControlOut=0.9;
  K_SpeedControlOut=0.2;
  delayms(100);
  K_AngleControlOut=0.85;
  K_SpeedControlOut=0.3;
  delayms(100);
  K_AngleControlOut=0.8;
  K_SpeedControlOut=0.5;
  delayms(100);
//  K_AngleControlOut=0.75;
//  K_SpeedControlOut=0.5;
  K_AngleControlOut=0.75;
  K_SpeedControlOut=0.9;
  delayms(100);
  Speed_Set=15;
  delayms(500);
  Dir_Control_P=32;
  Speed_Set=30;
  delayms(500);
  Speed_Set=45;
  delayms(500);
  Speed_Set=60;
  delayms(500);
  Speed_Set=70;
  delayms(500);
  led(LED2,LED_ON);

   Dir_Control_P=32;

  while(1); 
 }


