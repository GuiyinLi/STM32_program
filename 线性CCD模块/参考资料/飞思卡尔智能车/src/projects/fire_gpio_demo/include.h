#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include  "common.h"

/*
 * Include 用户自定义的头文件
 */

#include  "gpio.h"       //IO口操作
#include  "uart.h"      //串口
#include  "lptmr.h"     //低功耗定时器(延时)
#include  "led.h"     //低功耗定时器(延时)
#include  "5110.h"     
#include  "PIT.h"
#include "exti.h"
#include  "PIT.h"
#include  "AT24C02.h"
#include  "ftm.h"
#include  "spi.h"
#include "NRF24L0.h"
#include  "adc.h"
#include  "angle.h"
#include "math.h"
#include  "delay.h"

//#define Gravity_Offest  1952         //倾角中间值
#define Gravity_Max     2942         //倾角最大值
#define Gravity_Min     962          //倾角最小值
#define Gravity_Ratio   0.05787      //倾角比例值，换算成角度

#define Gyro_Ratio     0.000262      //陀螺仪换算比例值，换算成角度

#define  MOTOR_OUT_DEAD_VAL    0
#define  Speed_Control_Period 20    //速度控制周期，20毫秒
#define  SpeedOutLimit        18
#define  DirectionOutLimit    16




/*
const uint16   Gyto_Q =1;//卡尔曼滤波中的重要系数，调整波形
const uint16   Gravity_R =300;
volatile uint16 Gravity_Offest;    //重力加速度中间值
volatile uint16 Gravity_Max;       //重力加速度最大值
volatile uint16 Gravity_Min;       //重力加速度最小值
volatile float  Gravity_Ratio;     //重力加速度比例值
volatile uint16 Gyto0_Offest;      //陀螺仪0中间值
volatile float Gyto0_Angle_Ratio;  //陀螺仪0换算比例值，换算成弧度
volatile float Gyto0_Speed_Ratio;  //陀螺仪0换算比例值，包含角速度、阻尼系数
*/

#endif  //__INCLUDE_H__
