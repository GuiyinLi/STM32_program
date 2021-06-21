#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	  
#include "color.h"

#define FONT_1206		12
#define FONT_1608		16
#define FONT_1612		32
#define FONT_3216		64
//  引脚说明
//	SCL	->	PB0/PB9		<-	WR
//	SDA	->	PB1/PB10	<-	RD
//	CS	->	PB6
//	D/C	->	PB7
//	RES	->	PB8
                                  

//OLED模式设置
//0: 4线串行模式  （模块的BS1，BS2均接GND）
//1: 并行8080模式 （模块的BS1，BS2均接VCC）
#define OLED_MODE 0

#define Max_Column	0x5f			// 0-95
#define Max_Row			0x3f			// 0-63

#define Mid_Column	48				// 48
#define Mid_Row			32				// 32
#define Max_Width	  96		
#define Max_Height	64			
#define	Brightness	0x0F			  
//-----------------OLED端口定义----------------  					   

#define OLED_CS_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_6);	//CS->PB6 片选输入引脚 低电平有效
#define OLED_CS_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_6)

#define OLED_RS_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_7)	//D/C->PB7  data(1)/command(0)控制引脚
#define OLED_RS_Set() GPIO_SetBits(GPIOB,GPIO_Pin_7)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_8)	//RES->PB815 复位信号输入引脚
#define OLED_RST_Set() GPIO_SetBits(GPIOB,GPIO_Pin_8)

#if OLED_MODE==1
 
#define OLED_WR_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_9)  //R(1)/W(0)控制引脚
#define OLED_WR_Set() GPIO_SetBits(GPIOB,GPIO_Pin_14)

#define OLED_RD_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_10)
#define OLED_RD_Set() GPIO_SetBits(GPIOB,GPIO_Pin_10)

//PC0~7,作为数据线
#define DATAOUT(x) GPIO_Write(GPIOC,x);//输出
#else

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_0)//CLK   SCL->PB0
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_0)  

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_1)	//DIN SDA->PB1
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_1)

#endif

  
//使用4线串行接口时使用 
  
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//测试函数
void Screen_Init(void);
void OLED_Show_CCD(void);
//OLED控制用函数
void ssd1331_set_color(ColorType Color);
void ssd1331_draw_point(uint8_t chXpos, uint8_t chYpos, ColorType hwColor) ;
void ssd1331_clear_screen(ColorType Color);
void ssd1331_fill_black(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2,ColorType Color);
void ssd1331_copy(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 ,uint16_t _usY2,uint16_t _usX3 , uint16_t _usY3);
void ssd1331_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , ColorType _ucColor);
void ssd1331_draw_v_line(uint8_t chXpos, uint8_t chYpos, uint8_t chHeight, ColorType hwColor);
void ssd1331_draw_h_line(uint8_t chXpos, uint8_t chYpos, uint8_t chHeight, ColorType hwColor);
void ssd1331_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, ColorType _ucColor);
void ssd1331_display_char(uint8_t chXpos, uint8_t chYpos, uint8_t chChr, uint8_t chSize, ColorType hwColor,uint8_t mode);
void ssd1331_display_string(uint8_t chXpos, uint8_t chYpos, uint8_t *pchString, uint8_t chSize, ColorType hwColor,uint8_t mode);
void ssd1331_display_string_screen_center(uint16_t Width,uint16_t Height,uint8_t *pchString, uint8_t chSize, ColorType hwColor,uint8_t mode);
void ssd1331_display_Chinese(uint8_t chXpos, uint8_t chYpos, uint8_t no, ColorType hwColor);
u32 ssd1331_pow(u8 m,u8 n);
void ssd1331_display_number(u8 x,u8 y,u32 num,u8 len,u8 size,ColorType Color,uint8_t mode);
void ssd1331_draw_bitmap(uint8_t *pchBmp,uint8_t chXpos, uint8_t chYpos, ColorType hwColor);
void ssd1331_DrawRectangle(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2, ColorType _lineColor,ColorType _areaColor);
void ssd1331_draw_64Kpattern(uint8_t *pchBmp,uint8_t chXpos, uint8_t chYpos);
void ssd1331_vertical_scroll(uint8_t scrollingDirection, uint8_t row_perStep, uint16_t timeStepms);
void OLED_Init(void);

#endif  
	 



