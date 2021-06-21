#include "ssd1331.h"
#include "oled.h"
#include "color.h"
#include "delay.h"
void uOLED_Delay(unsigned char l)
{
	while(l--);
}

void OLED_Delay(unsigned char n)
{
unsigned char i,j,k;

	for(k=0;k<n;k++)
	{
		for(i=0;i<131;i++)
		{
			for(j=0;j<15;j++)
			{
				uOLED_Delay(203);	
			}
		}
	}
}

void Write_Command(unsigned char Command)
{
	unsigned char i;
	OLED_CS_Clr();  //拉低片选
	OLED_RS_Clr();  //写命令
	for (i=0; i<8; i++)
	{
		OLED_SCLK_Clr();
		if(Command&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		Command = Command << 1;
		OLED_SCLK_Set();
	}
	OLED_RS_Set();
	OLED_CS_Set();
}

void Write_Data(unsigned char Data)
{
	unsigned char i;
	OLED_CS_Clr();
	OLED_RS_Set(); //写数据
	for (i=0; i<8; i++)
	{
		OLED_SCLK_Clr();
		if(Data&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		Data = Data << 1;
		OLED_SCLK_Set();
	}
	OLED_RS_Set();
	OLED_CS_Set();
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//设置要显示的列(0-95 == 0x00-0x5F)
void Set_Column_Address(unsigned char start_column_address, unsigned char end_column_address)
{
	Write_Command(CMD_Set_Column_Address);			 	  //   Set Column Address   96*64
	Write_Command(start_column_address);				    //   Default => 0x00 (Start Address)
	Write_Command(end_column_address);				   		//   Default => 0x5F (End Address)
}
//设置要显示的行(0-62 == 0x00-0x5F)
void Set_Row_Address(unsigned char start_row_address, unsigned char end_row_address)
{
	Write_Command(CMD_Set_Row_Address);							// Set Row Address
	Write_Command(start_row_address);				    		//   Default => 0x00 (Start Address)
	Write_Command(end_row_address);				    			//   Default => 0x3F (End Address)
}
/**
 * @fun Set Re-Map / Color Depth A[7:0] 设置扫描格式以及颜色深度
 * @Default 0x40 从上到下 从左到右 RGB 65536色      
 * @A[0]	Horizontal Address Increment(0) Vertical address increment(1)
 * @A[1]	Column Address 0 Mapped to SEG0(0) Column Address 0 Mapped to SEG95(1) 
 * @A[2]	Color Order: RGB(0) Color Order: BGR(1)
 * @A[3]	Disable left-right swapping on COM(0)	 set left-right swapping on COM
 * @A[4]	Scan from COM0 to COM[N-1]	Scan from COM[N-1] to COM0
 * @A[5]	Disable COM Split Odd Even Enable COM Split Odd Even
 * @a[7:6] 00:256Colors  01:65536 Colors 11:65536 Colors2
 *
**/
void Set_Remap_Format(unsigned char d)
{
	Write_Command(CMD_Set_Remap_Format);
	Write_Command(d);				    							
}
//   Set Vertical Scroll by RAM
//   Default => 0x00   00-63D
//设置开始显示的行，以该行作为起点显示
void Set_Start_Line(unsigned char d)
{
	Write_Command(CMD_Set_Display_Start_Line);			  
	Write_Command(d);			    												
}
// Set Vertical Scroll by Row 显示偏移
// Default => 0x00    00-63D
//从起始行开始的偏移量，在偏移量处开始显示
void Set_Display_Offset(unsigned char d)
{
	Write_Command(CMD_Set_Display_Offset);				
	Write_Command(d);				   												
}
// Set Display Mode 0 1 2 3				         
// Default => 0xA4
// 0xA4 (0x00) => Normal Display  
// 0xA5 (0x01) => Entire Display On, All Pixels Turn On at GS Level 63
// 0xA6 (0x02) => Entire Display Off, All Pixels Turn Off
// 0xA7 (0x03) => Inverse Display
void Set_Display_Mode(unsigned char d)
{
	Write_Command(CMD_Set_Display_Mode|d);			  
}
// Set Display On/Off      
// Default => 0xAE
// 0xAE (0x00) => Display Off (Sleep Mode On)
// 0xAF (0x01) => Display On (Sleep Mode Off)
void Set_Display_On_Off(unsigned char d)
{
	Write_Command(CMD_Set_Display_On_Off|d);			
}
// Set Power Saving Mode
// Default => 0x1A
// 0x1A (0x00) => Enable
// 0X0B (0x01) => Disable
void Set_Power_Saving_Mode(unsigned char d)
{
	Write_Command(CMD_Set_Power_Saving_Mode);				
	Write_Command(d);
}
//复位和预上电周期  Set Reset (Phase1)/Pre-charge (Phase 2) period
// Default => 0x31 3个周期 1个周期
void Set_Reset_Pre_charge_period(unsigned char d)
{
	Write_Command(CMD_Set_Reset_Pre_charge_period);			
	Write_Command(d);			      														
}
// Default => 0xDO
// A[3:0] => Display Clock Divider 时钟分频
// A[7:4] => Oscillator Frequency  晶振频率
void Set_Oscillator_Frequency_Clock_Divider(unsigned char d)
{
	Write_Command(CMD_Set_Oscillator_Frequency_Clock_Divider);
	Write_Command(d);			
}
//设置预上电电压等级 Set Pre-charge Level
//Default => 0x3E (0.5*VCC)  三原色共用预上电电压等级
void Set_Pre_charge_Level(unsigned char d)
{
	Write_Command(CMD_Set_Pre_charge_Level);
	Write_Command(d);			    	
}
/**	
	*Set COM Deselect Voltage Level
	*Default => 0x3E (0.83*VCC)
	*A[5:1] Hex code V COMH
	*00000 00h 0.44 x VCC
	*01000 10h 0.52 x VCC
	*10000 20h 0.61 x VCC
	*11000 30h 0.71 x VCC
	*11111 3Eh 0.83 x VCC
**/
void Set_VCOMH(unsigned char d)
{
	Write_Command(0xBE);
	Write_Command(d);				   
}
//设置颜色对比度 三基色 A B C -> R G B
void Set_Contrast_Color(unsigned char a, unsigned char b, unsigned char c)
{
	Write_Command(CMD_Set_Contrast_Color_A);			
	Write_Command(a);				    										//   Default => 0x80 (Maximum)
  Write_Command(CMD_Set_Contrast_Color_B);		   
	Write_Command(b);				    										//   Default => 0x80 (Maximum)
  Write_Command(CMD_Set_Contrast_Color_C);			 
	Write_Command(c);				   										  //   Default => 0x80 (Maximum)
}
//设置二次预上电速度 Set Second Pre-charge Speed of Color
//设置的值应与颜色A、B、C的对比度匹配(Contrast_Color)
void Set_Second_Pre_charge_Speed_of_Color(unsigned char a, unsigned char b, unsigned char c)
{
	Write_Command(CMD_Set_Second_Pre_charge_Speed_of_Color_A);			
	Write_Command(a);																								//   Default => 0x80 (Color A)
  Write_Command(CMD_Set_Second_Pre_charge_Speed_of_Color_B);		
	Write_Command(b);																								//   Default => 0x80 (Color B)
  Write_Command(CMD_Set_Second_Pre_charge_Speed_of_Color_C);		
	Write_Command(c);																								//   Default => 0x80 (Color C)
}
//主电流值越小，OLED面板显示越暗。负责调节亮度
void Set_Master_Current(unsigned char d)
{
	Write_Command(CMD_Set_Master_Current);			// Master Contrast Current Control
	Write_Command(d);				  									//   Default => 0x0F (Maximum)
}
//This command selects the external VCC power supply. External VCC power should be connected to the VCC pin.
//A[0] bit must be set to 0b after RESET.
void Set_Master_Configuration(unsigned char d)
{
	Write_Command(CMD_Set_Master_Configuration);			  // Master Contrast Configuration
	Write_Command(d);				    												//   Default => 0x8E (Maximum)
}
//设置多路复用比
//例如，当多路复用比设置为16时，仅启用16个公共管脚。开始和结束
//启用的公共管脚取决于通过命令A2h编程的“显示偏移”寄存器的设置
//Default => 0x3F  N = A[5:0] from 15d to 63d(3Fh) A[5:0] from 00d to 14d are invalid entry(0-14为无效条目)
void Set_Multiplex_Ratio(unsigned char d)
{
	Write_Command(CMD_Set_Multiplex_Ratio);		
	Write_Command(d);				   
}
//Set Command Lock
// Default => 0x12
// 0x12 => Driver IC interface is unlocked from entering command.
// 0x16 => All Commands are locked except 0xFD.
void Set_Command_Lock(unsigned char d)
{
	Write_Command(CMD_Set_Command_Lock);
	Write_Command(d);					 	
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//填充使/失能
//A0 0 : Disable Fill for Draw Rectangle Command (RESET)
//0 A[4:0] * * * A4 0 0 0 A0 1 : Enable Fill for Draw Rectangle Command
//A[3:1] 000: Reserved values
//A4 0 : Disable reverse copy (RESET) Fill Enable /Disable
//1 : Enable reverse during copy command.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Enable_Disable(unsigned char a)
{ 
	Write_Command(CMD_Fill_Enable_Disable);	// Draw Line
  Write_Command(a);	    									// Column Address of Start
}
void Draw_Line(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f, unsigned char g)
{
	Write_Command(CMD_Draw_Line);		// Draw Line
	Write_Command(a);				// Column Address of Start
	Write_Command(b);				// Row Address of Start	
	Write_Command(c);       // Column Address of End 
	Write_Command(d);       // Row Address of End
	Write_Command(e);	      // Color C of the line
	Write_Command(f);       // Color B of the line
	Write_Command(g);       // Color A of the line
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Graphic Acceleration (Partial or Full Screen)
//
//A[6:0]: Column Address of Start
//B[5:0]: Row Address of Start
//C[6:0]: Column Address of End
//D[5:0]: Row Address of End
//E[5:1]: Color C of the line  0-32d
//F[5:0]: Color B of the line  0-64d
//G[5:1]: Color A of the line  0-32d
//H[5:1]: Color C of the fill area  0-32d
//I[5:0]: Color B of the fill area  0-64d
//J[5:1]: Color A of the fill area  0-32d
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Draw_Rectangle(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f, unsigned char g,unsigned char h,unsigned char i,unsigned char j)
{   
	Write_Command(CMD_Draw_Rectangle);		// Draw Rectangle
	Write_Command(a);											// Column Address of Start
	Write_Command(b);											// Row Address of Start	
	Write_Command(c);         						// Column Address of End 
	Write_Command(d);         						// Row Address of End
	Write_Command(e);	        						// Color C of the line
	Write_Command(f);         						// Color B of the line
	Write_Command(g);         						// Color A of the line
  Write_Command(h);         						// Color C of the fill area
  Write_Command(i);         						// Color B of the fill area
  Write_Command(j);         						// Color A of the fill area
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//A[6:0]: Column Address of Start
//B[5:0]: Row Address of Start
//C[6:0]: Column Address of End
//D[5:0]: Row Address of End
//E[6:0]: Column Address of New Start
//F[5:0]: Row Address of New Start
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void copy(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f)
{   
	Write_Command(CMD_Copy);	// copy
	Write_Command(a);					// Column Address of Start
	Write_Command(b);					// Row Address of Start	
	Write_Command(c);         // Column Address of End 
	Write_Command(d);         // Row Address of End
	Write_Command(e);	        // Column Address of New Start
	Write_Command(f);         // Row Address of New Start
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//	全填充
//    a: RRRRRGGG        0001 1111           10100000 00100000 11110000
//    b: GGGBBBBB				 1111 1000
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(unsigned char a, unsigned char b )
{
	unsigned char i,j;
	Set_Column_Address(0x00,Max_Column);
	Set_Row_Address(0x00,Max_Row);
	for(i=0;i<64;i++)
	{
		for(j=0;j<96;j++)
		{
			Write_Data(a);
			Write_Data(b);
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//	块填充
//    a: Column Address of Start
//    b: Column Address of End
//    c: Row Address of Start
//    d: Row Address of End
//    e: RRRRRGGG
//    f: GGGBBBBB
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f)
{
  unsigned char i,j;
	Set_Column_Address(a,b);
	Set_Row_Address(c,d);
	for(i=0;i<(d-c+1);i++)
	{
		for(j=0;j<(b-a+1);j++)
		{
			Write_Data(e);
			Write_Data(f);
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Checkboard (Full Screen)
//	黑白刷屏
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Checkerboard()
{
	unsigned char i,j;
	Set_Column_Address(0x00,Max_Column);
	Set_Row_Address(0x00,Max_Row);
	for(i=0;i<64;i++)
	{
		for(j=0;j<96;j++)
		{
			Write_Data(0xFF);
			Write_Data(0xFF);
			Write_Data(0x00);
			Write_Data(0x00);
		}
		for(j=0;j<96;j++)
		{
			Write_Data(0x00);
			Write_Data(0x00);
			Write_Data(0xFF);
			Write_Data(0xFF);
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Color Bar (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Rainbow(void)
{
	// White => Column 1~16
		Fill_Block(0x00,0x0c,0x00,Max_Row,0xFF,0xFF);

	// Yellow => Column 17~32
		Fill_Block(0x0c,0x18,0x00,Max_Row,0xFF,0xE0);

	// Purple => Column 33~48
		Fill_Block(0x18,0x24,0x00,Max_Row,0xF8,0x1F);

	// Cyan => Column 49~64
		Fill_Block(0x24,0x30,0x00,Max_Row,0x07,0xFF);

	// Red => Column 65~80
		Fill_Block(0x30,0x3c,0x00,Max_Row,0xF8,0x00);

	// Green => Column 81~96
		Fill_Block(0x3c,0x48,0x00,Max_Row,0x07,0xE0);

	// Blue => Column 97~112
		Fill_Block(0x48,0x54,0x00,Max_Row,0x00,0x1F);

	// Black => Column 113~128
		Fill_Block(0x54,Max_Column,0x00,Max_Row,0x00,0x00);
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Clear	Windows
//	只对图片有效，清楚后，清除的区域变为像素点
//	此函数执行一次
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Clear_Windows(uint8_t start_column,uint8_t start_row,uint8_t end_column,uint8_t end_row)
{
	Write_Command(CMD_Clear_Windows);		// Horizontal Scroll Setup
	Write_Data(start_column);
	Write_Data(start_row);
	Write_Data(end_column);
	Write_Data(end_row);
}
//矢量显示图片
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End
//    c: Row Address of Start
//    d: Row Address of End
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_64k_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
	unsigned char *Src_Pointer;
	unsigned int i,j;
	j=(b-a+1)*(d-c+1);
	Src_Pointer=Data_Pointer;
	   Set_Column_Address(a,b);
	Set_Row_Address(c,d);	
	for (i = 0; i<j; i++)
	{		
		Write_Data(*Src_Pointer);
		Src_Pointer++;
		Write_Data(*Src_Pointer);
		Src_Pointer++;
	}
}

void Show_256k_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
	unsigned char *Src_Pointer;
	unsigned char i,j;

	Src_Pointer=Data_Pointer;
	Set_Column_Address(a,b);
	Set_Row_Address(c,d);	

	for(i=0;i<(d-c+1);i++)
	{
		for(j=0;j<(b-a+1);j++)
		{
			Write_Data(*Src_Pointer);
			Src_Pointer++;
			Write_Data(*Src_Pointer);
			Src_Pointer++;
			Write_Data(*Src_Pointer);
			Src_Pointer++;
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical Scrolling (Full Screen)
//	上下滚屏 图片文字均可滚动
//    scrollingDirection: Scrolling Direction
//      								 "0x00" (Upward)
//      								 "0x01" (Downward)
//    row_perStep: Set Numbers of Row Scroll per Step 每次滚动的行数
//    timeStep: Set Time Interval between Each Scroll Step
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Vertical_Scroll(unsigned char scrollingDirection, unsigned char row_perStep, uint16_t timeStepms)
{
	unsigned int i;
	if(scrollingDirection)
	{
			for(i=0;i<96;i+=row_perStep)
			{
				Set_Start_Line(96-i);
				delay_ms(timeStepms);
			}
	}
	else
	{
			for(i=0;i<96;i+=row_perStep)
			{
				Set_Start_Line(i);
				delay_ms(timeStepms);
			}
	}
	Set_Start_Line(0x00);
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Automatic Horizontal Scrolling (Instruction)
//	该函数目前无法实现水平滚动 怀疑指令或者寄存器有问题
//    a: A[6:0]: Set number of column as horizontal scroll offset 横向滚动的每次的偏移量
//       Range: 0d-95d ( no horizontal scroll if  equals to 0)
//    b: B[5:0]: Define start row address	  0-63
//    c: C[6:0]: Set number of rows to be horizontal scrolled 	 0-95 滚动的总行数
//    d: D[5:0]: Set number of row as vertical scroll offset 	 0-63 纵向滚动的每次的偏移量
//       Range: 0d-63d ( no vertical scroll if equals to 0)
//    e: E[1:0]: Set time interval between each scroll	step
//       00b 6 frames
//       01b 10 frames
//       10b 100 frames
//       11b 200 frames
//    
//		 Sample code
//      27h // Continuous horizontal scroll
//
//    a:  01h // Horizontal scroll by 1 column
//    b:  28h // Define row 40 as start row address
//    c:  18h // Scrolling 24 rows
//    d:  00h // No vertical scroll
//    e:  00h // Set time interval between each
//               scroll step as 6 frames
//
//      2Fh // Activate scrolling
//	The parameters should not be changed after scrolling is activated
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/*void Horizontal_Scroll(unsigned char horizontal_offset_number, unsigned char start_row_address, unsigned char horizontal_rows_number, unsigned char vertical_offset_number, unsigned char scrollFrames)
{
	
	Write_Command(CMD_Horizontal_Vertical_Scroll);		// Horizontal Scroll Setup
	Write_Data(horizontal_offset_number);
	Write_Data(start_row_address);
	Write_Data(horizontal_rows_number);
	Write_Data(vertical_offset_number);
	Write_Data(scrollFrames);
	Write_Command(CMD_Activate_scrolling);						// Activate Horizontal Scroll
						
}*/
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	停止滚动
//  Deactivate Scrolling (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Deactivate_Scroll()
{
	Write_Command(CMD_Deactivate_Scroll);			// Deactivate Scrolling
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	渐变增量 
//  Fade In (Full Screen) 建议为200ms
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_In(uint16_t timems,unsigned char brightnessMax)
{
	unsigned int i;	

	Set_Display_On_Off(0x01);
	for(i=0;i<(brightnessMax+1);i++)
	{
		Set_Brightness(i);
		delay_ms(timems);
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	渐变变暗
//  Fade Out (Full Screen)	建议为200ms
//	brightnessMax要等于变暗前的亮度
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_Out(uint16_t timems,unsigned char brightnessMax)
{
	unsigned int i;	

	for(i=(brightnessMax+1);i>0;i--)
	{
		Set_Brightness(i-1);
		delay_ms(timems);
	}
	Set_Display_On_Off(0x00);
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Sleep Mode
//
//    "0x00" Enter Sleep Mode
//    "0x01" Exit Sleep Mode 唤醒之后，之前的显示数据保留
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Sleep_Wake(unsigned char a)
{
	if(a)
	{
		Set_Display_Mode(Normal_Display_Mode);
		Set_Display_On_Off(Display_On);
	}
	else
	{
		Set_Display_On_Off(Display_Off);
		Set_Display_Mode(Entire_Display_Off_Mode);
	}
}
//设置显示器的亮度 0-0x0f(15)
void Set_Brightness(unsigned char brightness)
{
	Set_Master_Current(brightness);
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// 	设置显示器的灰度表
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table()
{
	Write_Command(CMD_Set_Gray_Scale_Table);
	Write_Data(0x02);			// Gray Scale Level 1
	Write_Data(0x03);			// Gray Scale Level 2
	Write_Data(0x04);			// Gray Scale Level 3
	Write_Data(0x05);			// Gray Scale Level 4
	Write_Data(0x06);			// Gray Scale Level 5
	Write_Data(0x07);			// Gray Scale Level 6
	Write_Data(0x08);			// Gray Scale Level 7
	Write_Data(0x09);			// Gray Scale Level 8
	Write_Data(0x0A);			// Gray Scale Level 9
	Write_Data(0x0B);			// Gray Scale Level 10
	Write_Data(0x0C);			// Gray Scale Level 11
	Write_Data(0x0D);			// Gray Scale Level 12
	Write_Data(0x0E);			// Gray Scale Level 13
	Write_Data(0x0F);			// Gray Scale Level 14
	Write_Data(0x10);			// Gray Scale Level 15
	Write_Data(0x11);			// Gray Scale Level 16
	Write_Data(0x12);			// Gray Scale Level 17
	Write_Data(0x13);			// Gray Scale Level 18
	Write_Data(0x15);			// Gray Scale Level 19
	Write_Data(0x17);			// Gray Scale Level 20
	Write_Data(0x19);			// Gray Scale Level 21
	Write_Data(0x1B);			// Gray Scale Level 22
	Write_Data(0x1D);			// Gray Scale Level 23
	Write_Data(0x1F);			// Gray Scale Level 24
	Write_Data(0x21);			// Gray Scale Level 25
	Write_Data(0x23);			// Gray Scale Level 26
	Write_Data(0x25);			// Gray Scale Level 27
	Write_Data(0x27);			// Gray Scale Level 28
	Write_Data(0x2A);			// Gray Scale Level 29
	Write_Data(0x2D);			// Gray Scale Level 30
	Write_Data(0x30);			// Gray Scale Level 31
	Write_Data(0x33);			// Gray Scale Level 32
	Write_Data(0x36);			// Gray Scale Level 33
	Write_Data(0x39);			// Gray Scale Level 34
	Write_Data(0x3C);			// Gray Scale Level 35
	Write_Data(0x3F);			// Gray Scale Level 36
	Write_Data(0x42);			// Gray Scale Level 37
	Write_Data(0x45);			// Gray Scale Level 38
	Write_Data(0x48);			// Gray Scale Level 39
	Write_Data(0x4C);			// Gray Scale Level 40
	Write_Data(0x50);			// Gray Scale Level 41
	Write_Data(0x54);			// Gray Scale Level 42
	Write_Data(0x58);			// Gray Scale Level 43
	Write_Data(0x5C);			// Gray Scale Level 44
	Write_Data(0x60);			// Gray Scale Level 45
	Write_Data(0x64);			// Gray Scale Level 46
	Write_Data(0x68);			// Gray Scale Level 47
	Write_Data(0x6C);			// Gray Scale Level 48
	Write_Data(0x70);			// Gray Scale Level 49
	Write_Data(0x74);			// Gray Scale Level 50
	Write_Data(0x78);			// Gray Scale Level 51
	Write_Data(0x7D);			// Gray Scale Level 52
	Write_Data(0x82);			// Gray Scale Level 53
	Write_Data(0x87);			// Gray Scale Level 54
	Write_Data(0x8C);			// Gray Scale Level 55
	Write_Data(0x91);			// Gray Scale Level 56
	Write_Data(0x96);			// Gray Scale Level 57
	Write_Data(0x9B);			// Gray Scale Level 58
	Write_Data(0xA0);			// Gray Scale Level 59
	Write_Data(0xA5);			// Gray Scale Level 60
	Write_Data(0xAA);			// Gray Scale Level 61
	Write_Data(0xAF);			// Gray Scale Level 62
	Write_Data(0xB4);			// Gray Scale Level 63
}
void Set_Linear_Gray_Scale_Table()
{
	Write_Command(CMD_Set_Linear_Gray_Scale_Table);			// Default
}



