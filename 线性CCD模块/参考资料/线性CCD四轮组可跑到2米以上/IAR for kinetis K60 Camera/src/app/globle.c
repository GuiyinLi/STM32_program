#include "common.h"
#include "include.h"
#include "sci.h"
#include "isr.h"
#include "imgprocess.h"
#include "globle.h"
u8 img_byte_buff1[CAMERA_H][CAMERA_W]; //灰度图像存放位置

u8 line[80]={  30,31,32,33,34,      35,36,37,38,39,
               40,41,42,43,44,      45,46,47,48,49,
               50,51,52,53,54,      55,56,57,58,59,
               100,101,102,103,104, 105,106,107,108,109,
               110,111,112,113,114, 115,116,117,118,119,
               120,121,122,123,124, 125,126,127,128,129,
               130,131,132,133,134, 135,136,137,138,139,
               140,141,142,143,144, 145,146,147,148,149  };
volatile s16 servo_center=607;                 //舵机中心
volatile u32 motor_frequency=2000;              //motor频率
volatile s16 servo_step=23;                     //舵机步进值
volatile u8  now_clt=1;
volatile u8 flag_startsampling=0;
volatile u8 flag_start=1;
extern u8 start_a=0;
extern u8 ruwan=0,zhidao=0,wandao=0,s_wan=0,cross=0,out=0;
extern u8 chazhi_1[2]={0};