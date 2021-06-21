
#include "common.h"
#include "adc.h"
#include "calculation.h"
#include "uart.h"
#include  "FTM.h"
#include "i2c.h"
#include  "MMA.h"
#include  "MPU.h"
#include "gpio.h"

int g_fAngleControlOut = 0 ;
static double fGyroscopeAngleIntegral ;
extern float g_fDirectionControlOut;
extern float  CAR_ANGLE_SET;
extern float Direction_M_Old;
extern float Direction_M_New;
extern int g_nDirectionControlPeriod; 

extern int dier_count_period;
extern u8 Pixel[128];
extern u8 Pixel_d[127];
extern int blackline_track[30];

int dayulin_count;




/*速度控制参数*/
#define CAR_SPEED_SET	   SPEED_SET
#define MOTOR_OUT_DEAD_VAL   150//150//150
#define MOTOR_LEFT_SPEED_POSITIVE		(g_fLeftMotorOut > 0)
#define MOTOR_RIGHT_SPEED_POSITIVE		(g_fRightMotorOut > 0)
#define CAR_SPEED_CONSTANT			1000.0 /(SPEED_CONTROL_COUNT * CONTROL_PERIOD)/(float)OPTICAL_ENCODE_CONSTANT
#define OPTICAL_ENCODE_CONSTANT			157     //
#define SPEED_CONTROL_COUNT			20  	// 20 ms * 5 ms
#define SPEED_CONTROL_PERIOD			10


/***********速度控制*************/
extern float g_fCarSpeed;
extern float g_fCarOld;
extern float g_fCarNew;
extern int g_nSpeedControlPeriod;
extern int g_fSpeedOut;
extern int  GAR_SPEED_SETfDelta;
extern int g_nSpeedControlCount;
extern float g_fSpeedControlIntegral;
extern long g_nLeftMotorPulseSigma;
extern long g_nRightMotorPulseSigma;
extern float g_fSpeedControlOutOld;
extern float g_fSpeedControlOutNew;
extern float g_fSpeedControlOut;
extern long g_nLeftMotorPulse;
extern long g_nRightMotorPulse;
extern long  g_fLeftMotorOut;
extern long g_fRightMotorOut;
extern int Real_L_pulse;
extern int Real_R_pulse;
extern int GYRIO;
extern int Dirction_angledot_offset;




double fDeltaValue;
// #define CAR_ANGLE_SET					-20     
#define CAR_ANGLE_SPEED_SET                             0

  
s32 speed_Start;
float Speed_R;
float Speed_L; 
float Speed_R_Last;
float Speed_L_Last;
double jifenzhi=0; 

double  Gyro_Now;
double angle_offset_vertical;

/*****方向控制***************/
#define ABS(x) (((x) > 0) ? (x) : (-x))
extern float dir_gyro;
float DIR_CONTROL_P=-0.0030;//13.5;//-13.5  1800  15.8
float DIR_CONTROL_D=-0.005;//1.5;//-1.65


/*融合角度*/
float real_angle;
float real_interal=17;
double acce;
double gyro;
double get_real_angle( )
{
float Tz=3.7;
float real_value;
acce=z_angle();
gyro=-yy_anglespeed(); 
Gyro_Now=gyro;
real_angle=real_interal; 
real_value=(acce-real_angle)/Tz;
real_interal+=1.05*(real_value+gyro)*5/1000; 
real_angle=real_interal;  
return(real_angle);
}










/*********************************************************** 
函数名称：AngleAcceleration_init   
函数功能：加速度角度传感器初始化程序
入口参数：
出口参数：无 
备 注： 
***********************************************************/
void AngleAcceleration_init(void)
{
  

   //  adc_init(ADC1, AD10) ;     //PTB4 
     adc_init(ADC0, AD10) ;                       // PTA7
     adc_init(ADC1, AD11) ;                       // PTB5
     adc_init(ADC1, AD15) ;                       // PTB11 转向陀螺仪专用
  
     
 

}

/*********************************************************** 
函数名称：AngleAcceleration_AD   
函数功能：加速度角度传感器AD采集
入口参数：
出口参数：无 
备 注： 
***********************************************************/
void AngleAcceleration_AD(s16 *Average)
{

     
}


 //电机脉冲采集


void GetMotorPulse(void)       
{   
    
   

} 



//角速度和角度归一化处理//

 
void AD_Calculate()
 {
  
 }


//角度的融合计算//


void QingHua_AngleCalaulate(double G_angle,double Gyro)
 {
     
    
}

void tuoluoyijifen(double Gyro)
 {
     
    
}


//通过融合的角度，计算出赋予电机的pwm值//



















float AngleControl(double angle)
{
	float  fValue; 
       float  ANGLE_CONTROL_P=-80;//-70
       float  ANGLE_CONTROL_D=-6.3;//-6.3
    
      


	
	
	
	fValue = ((CAR_ANGLE_SET - angle) * ANGLE_CONTROL_P +             //角速度控制系数
	         (CAR_ANGLE_SPEED_SET - Gyro_Now) * ANGLE_CONTROL_D);     //角度控制系数
	
	
        

   	
return (fValue) ;

	
}

    




float fDelta_old,fDelta_now;
     float fP,fI,fD;
     float  SPEED_CONTROL_P=0.61;//0.05;
     float  SPEED_CONTROL_I=5;
void SpeedControl(void)
{  
       g_fCarSpeed= Real_L_pulse;
     fDelta_now=g_fCarSpeed-GAR_SPEED_SETfDelta;
     fP = fDelta_now * SPEED_CONTROL_P;
     fI = fDelta_now * SPEED_CONTROL_I;
     g_fSpeedControlIntegral+=fI;
if( g_fSpeedControlIntegral>1700)
       g_fSpeedControlIntegral=1700;
     if( g_fSpeedControlIntegral<-1700)
      g_fSpeedControlIntegral=-1700;
     g_fSpeedControlOutOld = g_fSpeedControlOutNew;
     g_fSpeedControlOutNew        =         fP     +    g_fSpeedControlIntegral;  
}




void SpeedControlOutput(void) 
{
	float fValue;
	fValue = g_fSpeedControlOutNew - g_fSpeedControlOutOld;
	g_fSpeedControlOut = fValue * (g_nSpeedControlPeriod+1 ) / SPEED_CONTROL_PERIOD + g_fSpeedControlOutOld;     
}


int unwork_pwm=35;//85;

//电机输出函数//
void MotorSpeedOut(float anglePWM )
 {
  int PWM;
  PWM=(int)anglePWM;
 if(PWM > 9000) PWM = 9000;
 if(PWM < -9000) PWM = -9000;
   if(PWM < 0) 
    {
     PWM   =0 - PWM;
     FTM_PWM_Duty(FTM0,CH5,0); 
     FTM_PWM_Duty(FTM0,CH0,0); 
     
     FTM_PWM_Duty(FTM0,CH6,PWM);
     FTM_PWM_Duty(FTM0,CH1,PWM);
    }
     else
    {    
     FTM_PWM_Duty(FTM0,CH6,0);
     FTM_PWM_Duty(FTM0,CH1,0) ;
     
     FTM_PWM_Duty(FTM0,CH5,PWM ); 
     FTM_PWM_Duty(FTM0,CH0,PWM);
     
    }
   

 }

void CCD1_init1(void)
{
  gpio_init (PORTE , 4, GPO,HIGH);
  gpio_init (PORTE , 5, GPO,HIGH);
  adc_init(ADC1, AD11) ;
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：CCD2_init
*  功能说明：CCD初始化
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void CCD2_init1(void)
{
  gpio_init (PORTE , 6, GPO,HIGH);
  gpio_init (PORTE , 7, GPO,HIGH);
  adc_init(ADC1, AD10) ;
  
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：StartIntegration
*  功能说明：CCD1启动程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void StartIntegration(void) {

    unsigned char i;

    SI_SetVal();
    SI_SetVal2();    /* SI  = 1 */
    SamplingDelay();
    CLK_SetVal(); 
    CLK_SetVal2();/* CLK = 1 */
    SamplingDelay();
    SI_ClrVal();   
    SI_ClrVal2();/* SI  = 0 */
    SamplingDelay();
    CLK_ClrVal();  
    CLK_ClrVal2();/* CLK = 0 */

    for(i=0; i<127; i++) {
        SamplingDelay();
        SamplingDelay();
        CLK_SetVal(); 
        CLK_SetVal2();/* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        CLK_ClrVal();    
        CLK_ClrVal2(); /* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();  
    CLK_SetVal2(); /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_ClrVal(); 
    CLK_ClrVal2();/* CLK = 0 */
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：StartIntegration
*  功能说明：CCD2启动程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void StartIntegration2(void) {

    unsigned char i;

    SI_SetVal2();            /* SI  = 1 */
    SamplingDelay();
    CLK_SetVal2();           /* CLK = 1 */
    SamplingDelay();
    SI_ClrVal2();            /* SI  = 0 */
    SamplingDelay();
    CLK_ClrVal2();           /* CLK = 0 */

    for(i=0; i<127; i++) {
        SamplingDelay();
        SamplingDelay();
        CLK_SetVal2();       /* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        CLK_ClrVal2();       /* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal2();           /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_ClrVal2();           /* CLK = 0 */
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：ImageCapture
*  功能说明：CCD1采样程序
*  参数说明：* ImageData   采样数组
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*ImageData =  ad_once(ADC1, AD6a, ADC_8bit);
*************************************************************************/

void ImageCapture(void) {

    unsigned char i;
    extern u8 Pixel[128]; 
    extern u8 Pixel2[128];
    u8 *a;
    u8 *b;
    a=Pixel;
    b=Pixel2;
    SI_SetVal();            /* SI  = 1 */
     SI_SetVal2();
    SamplingDelay();
    CLK_SetVal();           /* CLK = 1 */
    CLK_SetVal2();
    SamplingDelay();
    SI_ClrVal();            /* SI  = 0 */
    SI_ClrVal2();
    SamplingDelay();

    //Delay 10us for sample the first pixel
    /**/
    for(i = 0; i < 250; i++)
    {                    //更改250，让CCD的图像看上去比较平滑，
      SamplingDelay() ;  //200ns                  //把该值改大或者改小达到自己满意的结果。
    }

    //Sampling Pixel 1

    *a =  ad_once(ADC1, AD11, ADC_8bit);
    *b=ad_once(ADC1, AD10, ADC_8bit);
     a ++ ;
     b++;
    CLK_ClrVal();   
    
CLK_ClrVal2();        /* CLK = 0 */

    for(i=0; i<127; i++) {
        SamplingDelay();
        SamplingDelay();
        CLK_SetVal(); 
        CLK_SetVal2();/* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        //Sampling Pixel 2~128

       *a=  ad_once(ADC1, AD11, ADC_8bit);
        *b=  ad_once(ADC1, AD10, ADC_8bit);
        a ++ ;
        b++;
        CLK_ClrVal();  
        CLK_ClrVal2();/* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();  
     CLK_SetVal2();/* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_ClrVal();  
    CLK_ClrVal2(); /* CLK = 0 */
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：ImageCapture
*  功能说明：CCD2采样程序
*  参数说明：* ImageData   采样数组
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*ImageData =  ad_once(ADC1, AD6a, ADC_8bit);
*************************************************************************/

void ImageCapture2(unsigned char * ImageData) {

    unsigned char i;
    extern u8 AtemP ;

    SI_SetVal2();            /* SI  = 1 */
    SamplingDelay();
    CLK_SetVal2();           /* CLK = 1 */
    SamplingDelay();
    SI_ClrVal2();            /* SI  = 0 */
    SamplingDelay();

    //Delay 10us for sample the first pixel
    /**/
    for(i = 0; i < 250; i++) {                    //更改250，让CCD的图像看上去比较平滑，
      SamplingDelay() ;  //200ns                  //把该值改大或者改小达到自己满意的结果。
    }

    //Sampling Pixel 1

    *ImageData =  ad_once(ADC1, AD10, ADC_8bit);
    ImageData ++ ;
    CLK_ClrVal2();           /* CLK = 0 */

    for(i=0; i<127; i++) {
        SamplingDelay();
        SamplingDelay();
        CLK_SetVal2();       /* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        //Sampling Pixel 2~128

       *ImageData =  ad_once(ADC1, AD10, ADC_8bit);
        ImageData ++ ;
        CLK_ClrVal2();       /* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal2();           /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_ClrVal2();           /* CLK = 0 */
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：CalculateIntegrationTime
*  功能说明：计算曝光时间
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/

/* 曝光时间，单位ms */
u8 IntegrationTime = 10;
extern u8 Pixel[128];
extern u8 Pixel2[128];
void CalculateIntegrationTime(unsigned char * ImageData)
{

/* 128个像素点的平均AD值 */
u8 PixelAverageValue;
/* 128个像素点的平均电压值的10倍 */
u8 PixelAverageVoltage;
/* 设定目标平均电压值，实际电压的10倍 */
s16 TargetPixelAverageVoltage = 25;
/* 设定目标平均电压值与实际值的偏差，实际电压的10倍 */
s16 PixelAverageVoltageError = 0;
/* 设定目标平均电压值允许的偏差，实际电压的10倍 */
s16 TargetPixelAverageVoltageAllowError = 2;

    /* 计算128个像素点的平均AD值 */
    PixelAverageValue = PixelAverage(128,ImageData);
    /* 计算128个像素点的平均电压值,实际值的10倍 */
    PixelAverageVoltage = (unsigned char)((int)PixelAverageValue * 25 / 194);

    PixelAverageVoltageError = TargetPixelAverageVoltage - PixelAverageVoltage;
    if(PixelAverageVoltageError < -TargetPixelAverageVoltageAllowError)
    {
      PixelAverageVoltageError = 0- PixelAverageVoltageError ;
      PixelAverageVoltageError /= 2;
      if(PixelAverageVoltageError > 10 )
         PixelAverageVoltageError = 10 ;
       IntegrationTime -= PixelAverageVoltageError;
    }
    if(PixelAverageVoltageError > TargetPixelAverageVoltageAllowError)
    { 
        PixelAverageVoltageError /= 2;
        if(PixelAverageVoltageError > 10 )
           PixelAverageVoltageError = 10 ;
        IntegrationTime += PixelAverageVoltageError;}
 
    
  //  uart_putchar(UART0,0XAA) ;
    
 //   uart_putchar(UART0,PixelAverageVoltage) ;
 //   uart_putchar(UART0,PixelAverageVoltageError) ;
//    uart_putchar(UART0,IntegrationTime) ;
    if(IntegrationTime <= 1)
        IntegrationTime = 1;
    if(IntegrationTime >= 10)
        IntegrationTime = 10;
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：CalculateIntegrationTime2
*  功能说明：计算曝光时间
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/

/* 曝光时间，单位ms */
 
void CalculateIntegrationTime2(void) {
extern u8 Pixel2[128];
/* 128个像素点的平均AD值 */
u8 PixelAverageValue;
/* 128个像素点的平均电压值的10倍 */
u8 PixelAverageVoltage;
/* 设定目标平均电压值，实际电压的10倍 */
s16 TargetPixelAverageVoltage = 25;
/* 设定目标平均电压值与实际值的偏差，实际电压的10倍 */
s16 PixelAverageVoltageError = 0;
/* 设定目标平均电压值允许的偏差，实际电压的10倍 */
s16 TargetPixelAverageVoltageAllowError = 2;

    /* 计算128个像素点的平均AD值 */
    PixelAverageValue = PixelAverage(128,Pixel2);
    /* 计算128个像素点的平均电压值,实际值的10倍 */
    PixelAverageVoltage = (unsigned char)((int)PixelAverageValue * 25 / 194);

    PixelAverageVoltageError = TargetPixelAverageVoltage - PixelAverageVoltage;
    if(PixelAverageVoltageError < -TargetPixelAverageVoltageAllowError)
    {
      PixelAverageVoltageError = 0- PixelAverageVoltageError ;
      PixelAverageVoltageError /= 2;
      if(PixelAverageVoltageError > 10 )
         PixelAverageVoltageError = 10 ;
       IntegrationTime -= PixelAverageVoltageError;
    }
    if(PixelAverageVoltageError > TargetPixelAverageVoltageAllowError)
    { 
        PixelAverageVoltageError /= 2;
        if(PixelAverageVoltageError > 10 )
           PixelAverageVoltageError = 10 ;
        IntegrationTime += PixelAverageVoltageError;}
 
    
  //  uart_putchar(UART0,0XAA) ;
    
 //   uart_putchar(UART0,PixelAverageVoltage) ;
 //   uart_putchar(UART0,PixelAverageVoltageError) ;
//    uart_putchar(UART0,IntegrationTime) ;
    if(IntegrationTime <= 1)
        IntegrationTime = 1;
    if(IntegrationTime >= 100)
        IntegrationTime = 100;
}





/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：PixelAverage
*  功能说明：求数组的均值程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
u8 PixelAverage(u8 len, u8 *data) {
  unsigned char i;
  unsigned int sum = 0;
  for(i = 0; i<len; i++) {
    sum = sum + *data++;
  }
  return ((unsigned char)(sum/len));
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SendHex
*  功能说明：采集发数程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendHex(unsigned char hex) {
  unsigned char temp;
  temp = hex >> 4;
  if(temp < 10) {
    uart_putchar(UART2,temp + '0');
  } else {
    uart_putchar(UART2,temp - 10 + 'A');
  }
  temp = hex & 0x0F;
  if(temp < 10) {
    uart_putchar(UART2,temp + '0');
  } else {
   uart_putchar(UART2,temp - 10 + 'A');
  }
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SendImageData
*  功能说明：
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendImageData(unsigned char * ImageData) {

    unsigned char i;
    unsigned char crc = 0;

    /* Send Data */
    uart_putchar(UART2,'*');
    uart_putchar(UART2,'L');
    uart_putchar(UART2,'D');

    SendHex(0);
    SendHex(0);
    SendHex(0);
    SendHex(0);

    for(i=0; i<128; i++) {
     //SendHex(*ImageData++);
      SendHex((*ImageData)*1);
      ImageData++;
      
      
    }

    SendHex(crc);
    uart_putchar(UART2,'#');
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SamplingDelay
*  功能说明：CCD延时程序 200ns
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
 void SamplingDelay(void){
   volatile u8 i ;
   for(i=0;i<1;i++) {
    asm("nop");
    asm("nop");}
   
}

u8 tiaobian_count_l_near;
int input_last_l;
int out_near;
void lvbo_l_near(int input_l_near)                   //左右赛道去抖程序
{ 
    
 if((input_l_near-input_last_l)>20||(input_l_near-input_last_l)<-20)
    {
       out_near=input_last_l;
       tiaobian_count_l_near++;
    }
     else  out_near=input_l_near;
  if( tiaobian_count_l_near>=3)
    {
       tiaobian_count_l_near=0;
       out_near=input_l_near;
    }


 //input_last_l=out_near;
    
}
int input_last_r;
u8 tiaobian_count_r_near;
int  out_near_r;
void lvbo_r_near(int input_r_near)                    //左右赛道去抖程序
{

 if(   (input_r_near-input_last_r)>20 || (input_r_near-input_last_r)<-20    )
    {
      out_near_r=input_last_r;
      tiaobian_count_r_near++;
    }else out_near_r=input_r_near;
 if( tiaobian_count_r_near>=3)
 {
      tiaobian_count_r_near=0;
       out_near_r=input_r_near;    
 }
  


 
  

}

extern u8 guide_line_start;
extern u8 stop_flag;
extern int CarSpeedtarget;
extern u8 guideline_start_flag;
extern int zhijiao_count;
extern int zhilitime_count  ;
int un_S_flag;
u8 single_line_far_flag,single_line_near_flag;
int zhijiao_2ms_nb;
int Black_L,Black_R;
u8 _shizi_flag=0; 
u8 shizi_control=0;
#define shizi_flag 2
#define small_S_flag 3
#define large_S_flag 4
#define half_width_N 29
#define half_width_F 23
#define mag 1
#define jump_thre 5
u8 small_S_d_flag,large_S_d_flag;
u8 dangxian_flag;
int f_d_error,old_f_error;
u8 line_type_flag,lastline_type_flag;
float near_error_s[3],far_error_s[3];
float error,near_error,far_error,last_error_N,error_last;
int near_l_cd=128;
int near_r_cd=0;
int near_l_cd_last,near_r_cd_last;
int zhidao_count,shizi_count,small_S_count,large_S_count;
u8 thre_N=22;//25
u8 Thre_F=20;//25
u8 Thre=100;
u8 black_all_flag,zhijiao_left,zhijiao_right,zhijiao_count_l,zhijiao_count_r;
u8 line_lost_count,line_lost_flag;
u8 single_line_count;
u8 far_single_flag,near_single_flag;
float  error_s[20],error_sum,error_now;
float  error_s_far[20],error_sum_far,error_now_far;
u8 single_flag_near[20];
int near_l=0;
int near_r=128;
int far_l=0;
int far_r=128;
u8 zhijiao_l_count,zhijiao_r_count;
int zhijiao_turn_count=70;
int near_Track_central=64;
int near_track_cen_last=64;
int far_Track_central=64;
int far_Track_central_last=64;
int CCD_Thre=51;//51
int CCD_Thre_far=70;
int near_Black_L = 0; 
int far_Black_L = 0;
int near_Black_R = 128;
int far_Black_R = 128;
int track_width=52;

void ccd_pro(void)
{
u8 L_flag=0;
u8 R_flag=0;
u8 CCD_pic[128];
u8 CCD_pic_far[128];
long int sum;

sum=0;
for(u8 i=0;i<128;i++)
{sum+=Pixel[i];
}
CCD_Thre=sum/128;
CCD_Thre=CCD_Thre*1.3;
for(u8 i=0;i<128;i++)
{
  if(  (mag*Pixel[i])>=CCD_Thre   )
      CCD_pic[i]=1;
  else 
      CCD_pic[i]=0;
}



for( u8 i=near_Track_central;i>=4;i--)
  {
    if(CCD_pic[i+2]==1  && CCD_pic[i+1]==1  && CCD_pic[i+3]==1  
       &&CCD_pic[i-1]==0  && CCD_pic[i-2]==0   && CCD_pic[i-3]==0)
    { 
       near_Black_L=i;
       L_flag=1;
       break;
    }
  }
for(u8 i=near_Track_central;i<123;i++)
  {
     if( CCD_pic[i-3]==1  && CCD_pic[i-2]==1  && CCD_pic[i-1]==1  
        && CCD_pic[i+1]==0  && CCD_pic[i+2]==0   && CCD_pic[i+3]==0 )
    {
         near_Black_R=i;
         R_flag=1;
         break;
    }
  }



Black_L=near_Black_L;
Black_R=near_Black_R;
//近瞻处理
near_track_cen_last=near_Track_central;
near_Track_central=(near_Black_L+near_Black_R)/2;

if(L_flag==0 && R_flag==1)//左边线没有
  { 
    near_Track_central=near_Black_R-track_width/2;
  } 
if(L_flag==1 && R_flag==0)//右边线没有
  { 
    near_Track_central=near_Black_L+track_width/2;   
  }

if(   (near_track_cen_last-near_Track_central)>=10 || (near_track_cen_last-near_Track_central)<=-10)
   near_Track_central=near_track_cen_last;
//处理数据
  error=64-near_Track_central;
  if(near_Track_central>=120)
    near_Track_central=120;
  if(near_Track_central<=5)
    near_Track_central=5; 

/*****************************/
error_last=error_now;
for(u8 i=2;i>=1;i--)
{
  error_s[i]=error_s[i-1];
  error_sum+= error_s[i];
}
error_s[0]=error;
error_sum+=error_s[0];
error_now=error_sum*0.5;//*0.05;
error_sum=0;

Direction_M_Old=Direction_M_New;
Direction_M_New =DIR_CONTROL_P*error_now-DIR_CONTROL_D*(error_now-error_last);
}
 


void diretion_output()
{
 float fValue; 
fValue = Direction_M_New - Direction_M_Old;
g_fDirectionControlOut = fValue * (dier_count_period +1.0) /10.0 + Direction_M_Old;
}
  
  
  
void ima_pro()
  
 {
    int i,tpm;
    for(i=0;i<127;i++)
    {
      tpm=Pixel[i+1]-Pixel[i];
      if( tpm<0)tpm=ABS(tpm);
      Pixel_d[i]=tpm; 
      if(Pixel_d[i]<10)Pixel_d[i]=0;
      if(Pixel_d[i]!=0)
         { 
           dayulin_count++;
           blackline_track[dayulin_count]=i;///////未完待续
         }
    }
    for(i=0;i<127;i++)
      {
        if((Pixel[i+1]-Pixel[i])!=1);
      }
 }
  
void steering_control( float dir_control)//1.2为正中间，增大向左转,最大1.4，最小0.8左右
{
  //float i;
   //if(dir_control>=0.2) dir_control=0.2;
   //if(dir_control<=-0.4) dir_control=-0.4;
   //i=dir_control*10000.0/20.0;
   //i=1.2*10000.0/20.0+i;
 float n;
 //n=dir_control;
 //if(n>30) n=30;
 //if(n<-30) n=-30;
  // if(dir_control>=0.29) dir_control=0.29;
   //if(dir_control<=-0.49) dir_control=-0.49; 
 n=dir_control*10000.0/10.0;
 n=1.2*10000.0/10.0+n; 
 FTM_PWM_Duty(FTM1, CH1, (int)n);
}
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
