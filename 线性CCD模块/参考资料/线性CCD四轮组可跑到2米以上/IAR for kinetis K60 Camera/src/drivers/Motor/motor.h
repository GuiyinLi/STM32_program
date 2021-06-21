#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "common.h"
#include "FTM.h"

extern void setSpeed(uint16 l,uint16 r);
extern void init_motor();
//extern void setAngle(short int m);
extern void speedCorr_PID();
extern void setSpeedPWM(int16 m,int16 n);
#endif