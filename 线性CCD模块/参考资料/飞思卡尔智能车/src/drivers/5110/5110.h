
#ifndef __5110_H__
#define __5110_H__
/************************************************************************
*   主要使用的函数

*    显示一个整数0<x<14  0<Y<6
     LCD_printc_my(unsigned char x, unsigned char y, unsigned char c_dat)；

*    显示一个字母0<x<14  0<Y<6
     void LCD_printc(unsigned char x, unsigned char y, unsigned char c_dat)
      例如LCD_printc(unsigned char x, unsigned char y, 'A')

*   显示一个0-255的数0<x<14  0<Y<6   数据占3位
    LCD_display_8int(unsigned char x, unsigned char y, unsigned char c_dat) ;

*   显示一个0-65535的数0<x<14  0<Y<6 数据占5位
   LCD_display_16int(unsigned char x, unsigned char y, unsigned int c_dat);


*  
*  备    注：
*************************************************************************/

#include "common.h"
#include "gpio.h"

//*******************5110液晶配置*********************************
#define LCD_CLK_HIGH()          gpio_set (PORTA, 25, 1)
#define LCD_CLK_LOW()     	gpio_set (PORTA, 25, 0)

#define LCD_DIN_HIGH()    	gpio_set (PORTA, 26, 1)
#define LCD_DIN_LOW()      	gpio_set (PORTA, 26, 0)

#define LCD_DC_HIGH()      	gpio_set (PORTA, 27, 1)
#define LCD_DC_LOW()      	gpio_set (PORTA, 27, 0)

#define LCD_CS_HIGH()      	gpio_set (PORTA, 28, 1)
#define LCD_CS_LOW()     	gpio_set (PORTA, 28, 0)

#define LCD_RST_HIGH()     	gpio_set (PORTA, 29, 1)
#define LCD_RST_LOW()      	gpio_set (PORTA, 29, 0)


/*--------------------------------------------------------------*/
//指令宏定义
#define X_Col_Addr     0x80		//定位到第0列指令(列起始地址)(0 - 83)
#define Y_Page_Addr    0x40		//定位到第0页指令(页起始地址)(0 - 5)

/*--------------------------------------------------------------*/
//功能宏定义
//液晶复位
#define LCD_reset_hard	LCD_RST_LOW(); LCD_RST_HIGH()	//硬件复位

//便于理解
#define LCD_write_cmd(cmd)	LCD_write_byte(cmd, 0)	//写入指令
#define LCD_write_dat(dat)	LCD_write_byte(dat, 1)	//写入数据

/*--------------------------------------------------------------*/
//函数声明
void LCD_write_byte(unsigned char wbyte, unsigned char dat_cmd);//写入字节
//清屏参数(清DDRAM)
void LCD_clr_scr(void);
/*--------------------------------------------------------------*/
//显示清行
//num:	0 - 5
void LCD_clr_row(unsigned char num);
/*--------------------------------------------------------------*/
//液晶字节定位(1*1)
//液晶规划:
//x: 0 - 83
//y: 0 - 5
int LCD_pos_byte(int x, int y);
//显示数据
void LCD_display_16int(unsigned char x, unsigned char y, unsigned int c_dat);
void LCD_display_8int(unsigned char x, unsigned char y, unsigned char c_dat) ;
/*--------------------------------------------------------------*/
//液晶字符输出(6*8字体)
//x: 0 - 13
//y: 0 - 5
void LCD_printc(unsigned char x, unsigned char y, unsigned char c_dat);
void LCD_printc_my(unsigned char x, unsigned char y, unsigned char c_dat);
/*--------------------------------------------------------------*/
//液晶字符串输出(6*8字体)
//x: 0 - 13
//y: 0 - 5
void LCD_prints(unsigned char x, unsigned char y, unsigned char *s_dat);

/*--------------------------------------------------------------*/
//初始化LCD5110
void LCD5110_Init(void);				
/*--------------------------------------------------------------*/

#endif

