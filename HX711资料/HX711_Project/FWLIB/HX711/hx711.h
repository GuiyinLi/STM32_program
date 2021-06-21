#ifndef _hx711_h
#define _hx711_h
#include "sys.h"
#define DOUT PBin(6)     //数据线
#define SCLK PBout(7)    //时钟线

/* 数据线 时钟线 高低电平 */
#define DOUT_High   DOUT=1
#define DOUT_Low    DOUT=0
#define SCLK_High   SCLK=1
#define SCLK_Low    SCLK=0

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 370.0f

void HX711_Init(void);
long HX711_Read(void);
void Get_Gross_Weight(void);
void Get_Net_Weight(void);
long filter(void);
extern  long Gross_Weight ;   //毛重
extern long Net_Weight ;              //净重
extern u8 Flag_Error;
extern long Init_Gross_Weight;  //初始皮重
extern long All_Weight;
#endif


