

#ifndef __GLOBLE_H
#define __GLOBLE_H 

#include  "include.h"
#include  "ov7725.h"
extern u8 img_byte_buff1[CAMERA_H][CAMERA_W]; //ª“∂»ÕºœÒ¥Ê∑≈Œª÷√

extern volatile u8  now_clt;
extern volatile s16 servo_center;
extern volatile u32 motor_frequency;
extern volatile s16 servo_step;
extern u8 line[80];
extern volatile u8 flag_startsampling;
extern volatile u8 flag_start;
 extern u8 start_a;
 extern u8 ruwan,zhidao,wandao,s_wan,cross,out;
 extern u8 chazhi_1[2];
#endif  //__ISR_H

/* End of "isr.h" */