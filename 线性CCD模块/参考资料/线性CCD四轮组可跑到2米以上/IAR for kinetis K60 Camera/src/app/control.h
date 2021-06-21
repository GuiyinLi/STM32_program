#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "common.h"
#include "gpio.h"
#include "ov7725.h"
extern int Servo_PD(int Kp,int Kd,int set,int input);
extern u8 Motor_PI(s16 *Kp,s16 *Ki,s16 *set,s16 *Nowspeed);
extern s16 getangle(s16 row);
extern void resetpd(void);
extern int Nowoutput;
#endif  //__CONTROL_H__