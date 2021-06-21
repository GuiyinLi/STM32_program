#ifndef __CCD_H
#define __CCD_H	 
#include "sys.h"

/*	接线说明
		AO 	-->	PA1
		CLK -->	PA2
		SI	-->	PA3
*/
#define TSL_SI    PAout(3)  
#define TSL_CLK   PAout(2)  
#define Exposure_Time  10   //曝光时间(us)  最大为100ms
#define Dealy		Dly_us(Exposure_Time)		//Dly_us(Exposure_Time)  delay_us(Exposure_Time)

#define Single_Line		0
#define DMA_Tran      0    //CCD数据采集是否使用DMA传输 目前采用DMA传输数据会丢失

typedef float ccdData_Type; 
extern 	u16 ccd_adc;
extern 	ccdData_Type ccd_piexl[128];
extern 	ccdData_Type Median,Threshold;
extern 	ccdData_Type Left_Side,Right_Side;
void CCD_Init(void);
void Dly_us(int a);
void RD_TSL(void);
void getCCDMedian(void);

#endif
