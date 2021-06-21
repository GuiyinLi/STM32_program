#ifndef __ssd1331_H
#define __ssd1331_H		
#include "sys.h"
#include "color.h"

#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))

//基础指令集
#define CMD_Set_Column_Address														0x15
#define CMD_Draw_Line																		  0X21
#define CMD_Draw_Rectangle																0X22
#define CMD_Copy																					0X23
#define CMD_Clear_Windows																	0X25
#define CMD_Fill_Enable_Disable														0X26
#define CMD_Horizontal_Vertical_Scroll										0X27   //默认上下滚动										
#define CMD_Deactivate_Scroll															0X2E
#define CMD_Activate_scrolling														0X2F
#define CMD_Set_Row_Address      		  										0x75
#define CMD_Set_Contrast_Color_A													0X81
#define CMD_Set_Contrast_Color_B													0X82
#define CMD_Set_Contrast_Color_C													0X83
#define CMD_Set_Master_Current													  0X87
#define CMD_Set_Second_Pre_charge_Speed_of_Color_A        0X8A
#define CMD_Set_Second_Pre_charge_Speed_of_Color_B        0X8B
#define CMD_Set_Second_Pre_charge_Speed_of_Color_C        0X8C
#define CMD_Set_Remap_Format															0XA0
#define CMD_Set_Display_Start_Line												0XA1
#define CMD_Set_Display_Offset														0XA2
#define CMD_Set_Display_Mode															0XA4
#define CMD_Set_Multiplex_Ratio														0XA8
#define CMD_Set_Master_Configuration										  0XAD
#define CMD_Set_Display_On_Off														0xAE
#define CMD_Set_Power_Saving_Mode													0XB0
#define CMD_Set_Reset_Pre_charge_period 									0XB1
#define CMD_Set_Oscillator_Frequency_Clock_Divider				0XB3
#define CMD_Set_Gray_Scale_Table													0XB8
#define CMD_Set_Linear_Gray_Scale_Table										0XB9
#define CMD_Set_Pre_charge_Level													0XBB
#define CMD_Set_COM_Deselect_Voltage_Level								0XBE
#define CMD_Set_Command_Lock															0XFD

//高级指令集
#define Clear_Screen 																			Fill_RAM(0x00,0x00)
#define	Upward																						0x00
#define	Downward																					0x01
#define	Frames_6																					0x00
#define	Frames_10																					0x01
#define Frames_100																				0x02
#define Frames_200																				0x03
#define Sleep																							0x00
#define Wake																							0x01
#define Normal_Display_Mode															  0x00
#define Entire_Display_On_Mode													  0x01
#define	Entire_Display_Off_Mode														0x02
#define Inverse_Display_Mode															0x03
#define Display_On																				0x01
#define	Display_Off																				0x00
#define Lowest_Brightness																	0x00
#define Lower_Brightness																	0x02
#define Low_Brightness																		0x04
#define	Middle_Brightness																	0x06
#define	High_Brightness																		0x08
#define	Higher_Brightness																	0x0A
#define Highest_Brightness																0x0C
#define DisOverlay_Display                                0x00  //非叠加显示
#define Overlay_Display                                   0x01  //叠加显示
//测试程序
void Test(void);
//延时函数
void uOLED_Delay(unsigned char l);
void OLED_Delay(unsigned char n);
//基础指令函数
void Write_Command(unsigned char Command);
void Write_Data(unsigned char Data);
void Set_Column_Address(unsigned char start_column_address, unsigned char end_column_address);
void Set_Row_Address(unsigned char start_row_address, unsigned char end_row_address);
void Set_Remap_Format(unsigned char d);
void Set_Start_Line(unsigned char d);
void Set_Display_Offset(unsigned char d);
void Set_Display_Mode(unsigned char d);
void Set_Display_On_Off(unsigned char d);
void Set_Power_Saving_Mode(unsigned char d);
void Set_Reset_Pre_charge_period(unsigned char d);
void Set_Oscillator_Frequency_Clock_Divider(unsigned char d);
void Set_Pre_charge_Level(unsigned char d);
void Set_VCOMH(unsigned char d);
void Set_Contrast_Color(unsigned char a, unsigned char b, unsigned char c);
void Set_Second_Pre_charge_Speed_of_Color(unsigned char a, unsigned char b, unsigned char c);
void Set_Master_Current(unsigned char d);
void Set_Master_Configuration(unsigned char d);
void Set_Multiplex_Ratio(unsigned char d);
void Set_Command_Lock(unsigned char d);
//显示指令函数
void Fill_Enable_Disable(unsigned char a);
void Draw_Line(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f, unsigned char g);
void Draw_Rectangle(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f, unsigned char g,unsigned char h,unsigned char i,unsigned char j);
void Fill_RAM(unsigned char a, unsigned char b);
void copy(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f);
void Fill_RAM(unsigned char a, unsigned char b);
void Fill_Block(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f);
void Checkerboard(void);
void Rainbow(void);
void Clear_Windows(uint8_t start_column,uint8_t start_row,uint8_t end_column,uint8_t end_row);
void Show_64k_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void Show_256k_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void Vertical_Scroll(unsigned char scrollingDirection, unsigned char row_perStep, uint16_t timeStepms);
void Horizontal_Scroll(unsigned char horizontal_offset_number, unsigned char start_row_address, unsigned char horizontal_rows_number,uint16_t timeStep);
void Fade_In(uint16_t timems,unsigned char brightnessMax);
void Fade_Out(uint16_t timems,unsigned char brightnessMax);
void Set_Brightness(unsigned char brightness);
void Sleep_Wake(unsigned char a);


#endif
