//上电时候，CS、WR、RD 应该是高电平
//P0 为输出数据，这里还没设置方向的哦。
//P1 为输入数据，这里还没设置方向的哦。


#include "common.h"
#include "gpio.h"
#include "BL144002.h"


void lcd_io_init()
{
	gpio_init  (PORTD, 0, GPO, 0);
	gpio_init  (PORTD, 1, GPO, 0);
	gpio_init  (PORTD, 2, GPO, 0);
	gpio_init  (PORTD, 3, GPO, 0);
	gpio_init  (PORTD, 4, GPO, 0);
	gpio_init  (PORTD, 5, GPO, 0);
	gpio_init  (PORTD, 6, GPO, 0);
	gpio_init  (PORTD, 7, GPO, 0);

	gpio_init  (PORTC, 9, GPO, 0);
	gpio_init  (PORTC, 10, GPO, 0);
	gpio_init  (PORTC, 11, GPO, 0);
	gpio_init  (PORTC, 12, GPO, 0);
    gpio_init  (PORTC, 19, GPO, 0);
}


u8  rd_data(void)
{
    u8 data;
    wr=1;
    rs=1;
    cs=0;

    rd=0;     		//产生一个上升沿，
	BL14002_DELAY();
	PDDR_IN();		//设置为输入
    data=PIN;  		//读取低8位数据  
    rd=1;
    cs=1;
	PDDR_OUT();		//切换为输出
	
    return  data;
}



/*-----------------------------------------------LSI INITIAL----------------------------------------------------*/
//LCD初始化
void lcd_initialize()
{
	lcd_io_init();
	
    reset=1;
    DELAY_MS(100);
    reset=0;
    DELAY_MS(100);
    reset=1;
    DELAY_MS(500);		//上电给足够时间

    LCD_WR_CMD(0x11);   	//Sleep out   退出睡眠模式
    DELAY_MS(120);    //Delay 120ms
    //------------------------------------ST7735S Frame Rate-----------------------------------------//
    /* Set the frame frequency of the full colors normal mode. */
    LCD_WR_CMD(0xB1);     //In Normal Mode (Full Colors)    全屏模式
    LCD_WR_Data(0x05);    //设置 RTNA Set 1-line  Period  一行周期
    LCD_WR_Data(0x3A);    //设置 FPA: Front Porch   
    LCD_WR_Data(0x3A);    //设置 BPA: Back Porch 
                      //Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))  
                      //其中 fosc = 850kHz 
                      
    /* Set the frame frequency of the Idle mode.  */
    LCD_WR_CMD(0xB2);     //In Idle Mode (8-colors)
    LCD_WR_Data(0x05);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x3A);

    /* Set the frame frequency of the Partial mode/ full colors. */
    LCD_WR_CMD(0xB3);
    LCD_WR_Data(0x05);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x05);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x3A);
    
    //------------------------------------End ST7735S Frame Rate-----------------------------------------//
    LCD_WR_CMD(0xB4);   //Display Inversion Control  显示反转控制
    
    LCD_WR_Data(0x07);  //LCD_WR_Data(0x03); 原来为3，改为7
                    // 低三位从高到低，分别为  full colors normal mode 、Idle mode 、
                    // full Colors partial mode  的 点反转 或 列反转 ，1为列反转
                   
    LCD_WR_CMD(0xC0);   //Power Control Setting  电源控制设置
    LCD_WR_Data(0x28);
    LCD_WR_Data(0x08);
    LCD_WR_Data(0x84);
    LCD_WR_CMD(0xC1);
    LCD_WR_Data(0XC0);
    LCD_WR_CMD(0xC2);
    LCD_WR_Data(0x0C);
    LCD_WR_Data(0x00);
    LCD_WR_CMD(0xC3);
    LCD_WR_Data(0x8C);
    LCD_WR_Data(0x2A);
    LCD_WR_CMD(0xC4);
    LCD_WR_Data(0x8A);
    LCD_WR_Data(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCD_WR_CMD(0xC5); //  VCOM 电压配置
    LCD_WR_Data(0x0C);//  -0.725 
    LCD_WR_CMD(0x36); // 内存数据访问控制  MX 列地址顺序, MY 行地址顺序 ，MV 行列交换 ，ML 垂直刷新顺序 ,RGB  RGB-BGA顺序
    LCD_WR_Data(0xC8);//08
    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCD_WR_CMD(0xE0);
    LCD_WR_Data(0x05);
    LCD_WR_Data(0x1A);
    LCD_WR_Data(0x0C);
    LCD_WR_Data(0x0E);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x34);
    LCD_WR_Data(0x2D);
    LCD_WR_Data(0x2F);
    LCD_WR_Data(0x2D);
    LCD_WR_Data(0x2A);
    LCD_WR_Data(0x2F);
    LCD_WR_Data(0x3C);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x01);
    LCD_WR_Data(0x02);
    LCD_WR_Data(0x10);
    LCD_WR_CMD(0xE1);
    LCD_WR_Data(0x04);
    LCD_WR_Data(0x1B);
    LCD_WR_Data(0x0D);
    LCD_WR_Data(0x0E);
    LCD_WR_Data(0x2D);
    LCD_WR_Data(0x29);
    LCD_WR_Data(0x24);
    LCD_WR_Data(0x29);
    LCD_WR_Data(0x28);
    LCD_WR_Data(0x26);
    LCD_WR_Data(0x31);
    LCD_WR_Data(0x3B);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x03);
    LCD_WR_Data(0x12);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
     
    LCD_WR_CMD(0x3A); //65k mode
    LCD_WR_Data(0x05);
    LCD_WR_CMD(0x29); //开显示 Display on
    LCD_WR_CMD(0x2c); //

    LCD_WR_CMD(0x2a);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x02);	
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x81);

    LCD_WR_CMD(0x2B);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x03);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x82);

    DELAY_MS(200);
}

//开窗——Partial Mode 
void LCD_PTLON(Site_type site,Size_type size)
{	
    LCD_WR_CMD(0x2a);   //Partial Mode On  局部模式
    LCD_WR_Data((u8)(site.x>>8));  //高8位 Sx
    LCD_WR_Data((u8)site.x);	  //低8位 Sx
    LCD_WR_Data((u8)((site.x+size.W-1)>>8)); 
    LCD_WR_Data((u8)(site.x+size.W-1));

    LCD_WR_CMD(0x2B);//Row Address Set  行地址设置
    LCD_WR_Data((u8)(site.y>>8));
    LCD_WR_Data((u8)site.y);
    LCD_WR_Data((u8)((site.y+size.H-1)>>8));
    LCD_WR_Data((u8)(site.y+size.H-1));
}




