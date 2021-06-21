#ifndef __SERVO_H__
#define __SERVO_H__

#include "common.h"
#include "gpio.h"
#include "ov7725.h"
extern void Servo_init(void);
extern void Handle(s16 angle);
#endif  //__SERVO_H__