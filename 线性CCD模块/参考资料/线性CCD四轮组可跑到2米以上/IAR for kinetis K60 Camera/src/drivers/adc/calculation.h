


#ifndef  calculation_H
#define  calculation_H  

/*********************************************************************************************************
* Description: function
*********************************************************************************************************/
double get_real_angle( );






















/**********
加速度传感器和陀螺仪
**********/
void AngleAcceleration_init(void) ;    //加速度角度传感器初始化程序
void AngleAcceleration_AD(s16 *Average) ;    //加速度角度传感器AD采集
void AD_Calculate();//角速度和角度归一化处理//
void QingHua_AngleCalaulate(double G_angle,double Gyro);//角度的融合计算
float AngleControl(double angle);//通过融合的角度，计算出赋予电机的pwm值
void MotorSpeedOut(float anglePWM );//电机输出函数
void tuoluoyijifen(double Gyro);//陀螺仪积分计算
void SpeedControl(void);
void SpeedControlOutput(void) ;
/*********************************************************************************************************
* Description: define
*********************************************************************************************************
*/
 /**********************
 * extern variable
 *********************/

 extern s16  AAngleAccele[6] ;
 extern s16 g_fCarAngle ;

 extern double Gyro_Now;
 extern double angle_offset_vertical;
 extern double jifenzhi;
// extern double g_fGyroscopeAngleIntegral;
static double fGyroscopeAngleIntegral ;
#define GYRO_VAL					(AAngleAccele[1])         //加速度
#define MMA7361_vertical				(AAngleAccele[3])      //角度 
extern  float  Speed_R_Last;
extern  float Speed_L_Last;
extern  float Speed_R;
extern  float Speed_L;
extern  s32 speed_Start ;
 /**********************
 * AngleCalculate
 *********************/
#define  ADCOUT                                         3
#define  GYROSCOPE_ANGLE_SIGMA_FREQUENCY	      200 
#define  MOTOR_DEAD_VAL_R                              0
#define  MOTOR_DEAD_VAL_L                               0
//#define TRUE        1

#define SI_SetVal()   PTE4_OUT = 1
#define SI_ClrVal()   PTE4_OUT = 0
#define CLK_ClrVal()  PTE5_OUT = 0
#define CLK_SetVal()  PTE5_OUT = 1

#define SI_SetVal2()   PTE6_OUT = 1
#define SI_ClrVal2()   PTE6_OUT = 0
#define CLK_ClrVal2()  PTE7_OUT = 0
#define CLK_SetVal2()  PTE7_OUT = 1
void StartIntegration(void) ;
void StartIntegration2(void);
void ImageCapture(void) ;
void ImageCapture2(unsigned char * ImageData);
void SendHex(unsigned char hex) ;
void SamplingDelay(void) ;
void CCD1_init1(void) ;
void CCD2_init1(void) ;
void CalculateIntegrationTime(unsigned char * ImageData) ;
void CalculateIntegrationTime2(void);
u8 PixelAverage(u8 len, u8 *data) ;
void SendImageData(unsigned char * ImageData) ;


void ccd_pro(void);
void diretion_output();
void ima_pro();
void lvbo_r_near(int input_r_near);
void lvbo_l_near(int input_l_near);
void steering_control( float dir_control);



#endif
