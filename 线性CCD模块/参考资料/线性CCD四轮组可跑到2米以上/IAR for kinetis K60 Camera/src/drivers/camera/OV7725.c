#include "common.h"
#include "gpio.h"
#include "exti.h"
#include "dma.h"
#include "SCCB.h"
#include "OV7725.h"
#include "OV7725_REG.h"
#include "sci.h"
#include "isr.h"

#define OV7725_Delay_ms(time)  DELAY_MS(time)

#define OV7725_ID           0x21
//extern u8 const data_table[CAMERA_H]={46,53,58,60,66,68,83,97,109,120,130,139,147,
                                    // 155,163,170,178,186,192,197,202,206,210,
                                     // 214,218,222,225,228,231,234};             //需采集数据的行，2cm一行实际测得
u8 volatile * IMG_BUFF;
u8 volatile img_flag = IMG_FINISH;	
u8 volatile v_cnt=0;
static u8 Ov7725_reg_Init(void);

void Ov7725_exti_Init()
{
    //IMG_BUFF = imgbuff;
	//DMA通道0初始化，PTB8上升沿触发DMA传输,即为PCLK，源地址为PTD_BYTE0_IN，即为Y0-Y7(PTB0-PTB7)，目的地址为：BUFF ，每次传输1Byte，传输CAMERA_W次后停止传输，保存目的地址不变	
        img_flag = IMG_FINISH;		//图像状态
	DMA_PORTx2BUFF_Init(CAMERA_DMA_CH, (void *)&PTD_BYTE0_IN, (void *)IMG_BUFF, PTC0, DMA_BYTE1, H , DMA_falling_down_keepon);
	
	DMA_DIS(CAMERA_DMA_CH);		//先关闭 DMA,场行中断后启动DMA中断
	DMA_IRQ_DIS(CAMERA_DMA_CH);	//不需要使用DMA中断,由行中断来控制DMA传输

	exti_init(PORTA,26,rising_down);	//hang中断，内部下拉，上升沿触发中断
	exti_init(PORTB,0,rising_down); 	//chang中断，内部上拉，下降沿触发中断
        disable_irq(87); //关闭PTA的中断
        disable_irq(88);
}

void Ov7620_exti_Init()
{
    //IMG_BUFF = imgbuff;
	//DMA通道0初始化，PTB8上升沿触发DMA传输,即为PCLK，源地址为PTD_BYTE0_IN，即为Y0-Y7(PTB0-PTB7)，目的地址为：BUFF ，每次传输1Byte，传输CAMERA_W次后停止传输，保存目的地址不变	
        img_flag = IMG_FINISH;		//图像状态
	DMA_PORTx2BUFF_Init(CAMERA_DMA_CH, (void *)&PTD_BYTE0_IN, (void *)IMG_BUFF, PTC0, DMA_BYTE1, H , DMA_falling_down_keepon);
	
	DMA_DIS(CAMERA_DMA_CH);		//先关闭 DMA,场行中断后启动DMA中断
	DMA_IRQ_DIS(CAMERA_DMA_CH);	//不需要使用DMA中断,由行中断来控制DMA传输

	exti_init(PORTA,26,rising_down);	//hang中断，内部下拉，上升沿触发中断
	exti_init(PORTB,0,rising_down); 	//chang中断，内部上拉，下降沿触发中断
        disable_irq(87); //关闭PTA的中断
        disable_irq(88);
}






void ov7725_get_img()
{
    img_flag = IMG_START;					//开始采集图像
    PORTA_ISFR=~0;							//写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
    enable_irq(87); 						//允许PTA的中断
    while(img_flag != IMG_FINISH)           //等待图像采集完毕
    {
       if(img_flag == IMG_FAIL)            //假如图像采集错误，则重新开始采集
        {  
            img_flag = IMG_START;			//开始采集图像
            PORTA_ISFR=~0;					//写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
            enable_irq(87); 				//允许PTA的中断
        }
    }
       
       disable_irq(87); //关闭场中的，等待下一次场中断来到重新打开 
       
}


/*OV7725初始化配置表*/
Register_Info ov7727_reg[] =
{

    //寄存器，寄存器值次
    {COM4         ,0xC1},
    {CLKRC        ,0x00}, 
    {COM3         ,0xD0}, 
    {COM5         ,0x00},
    {COM7         ,0x40},  
    {HSTART       ,0x3F},
    {HSIZE        ,0x50}, 
    {VSTRT        ,0x03},
    {VSIZE        ,0x78},
    {HREF         ,0x00}, 
    {SCAL0        ,0x0A},
    {AWB_Ctrl0    ,0xE0},
	{DSPAuto      ,0xff},
    {DSP_Ctrl2    ,0x0C}, 
    {DSP_Ctrl3    ,0x00}, 
    {DSP_Ctrl4    ,0x00},
    {HOutSize     ,0x14},
    {VOutSize     ,0x1E},
    {EXHCH        ,0x00},
	{GAM1         ,0x0c},
	{GAM2         ,0x16},
	{GAM3         ,0x2a},
	{GAM4         ,0x4e},
	{GAM5         ,0x61},
	{GAM6         ,0x6f},
	{GAM7         ,0x7b},
	{GAM8         ,0x86},
	{GAM9         ,0x8e},
	{GAM10        ,0x97},
	{GAM11        ,0xa4},
	{GAM12        ,0xaf},
	{GAM13        ,0xc5},
	{GAM14        ,0xd7},
	{GAM15        ,0xe8},
	{SLOP         ,0x20},
	{LC_RADI      ,0x00},
	{LC_COEF      ,0x13},
	{LC_XC        ,0x08},
	{LC_COEFB     ,0x14},
	{LC_COEFR     ,0x17},
	{LC_CTR       ,0x05},
	{BDBase       ,0x99},
	{BDMStep      ,0x03},
    {SDE          ,0x04},
    {BRIGHT       ,0x00},                              
    {CNST         ,0xFF},
    {SIGN         ,0x06},
    {UVADJ0       ,0x11},
    {UVADJ1       ,0x02},
    
};

u8 cfgnum = sizeof(ov7727_reg)/sizeof(ov7727_reg[0]);   /*结构体数组成员数目*/



/************************************************
 * 函数名：Ov7725_Init
 * 描述  ：Sensor初始化
 * 输入  ：无
 * 输出  ：返回1成功，返回0失败
 * 注意  ：无
 ************************************************/
u8 Ov7725_Init(u8 *imgaddr)
{
          IMG_BUFF = imgaddr;
	//while(Ov7725_reg_Init() == 0);
	  Ov7725_exti_Init();
         return 0;
}

/************************************************
 * 函数名：Ov7725_reg_Init
 * 描述  ：Sensor 寄存器 初始化
 * 输入  ：无
 * 输出  ：返回1成功，返回0失败
 * 注意  ：无
 ************************************************/
u8 Ov7725_reg_Init(void)
{
    u16 i = 0;
    u8 Sensor_IDCode = 0;
	SCCB_GPIO_init();
	
	OV7725_Delay_ms(50);
        while( 0 == SCCB_WriteByte ( 0x12, 0x80 ) ) /*复位sensor:reset all of the register value */
    {
        i++;
		if(i == 20)
		{
			DEBUG_OUT("警告:SCCB写数据错误");
			//OV7725_Delay_ms(50);
			return 0 ;
		}
		
    }
    OV7725_Delay_ms(50);
    if( 0 == SCCB_ReadByte( &Sensor_IDCode, 1, 0x0b ) )	 /* 读取sensor ID号*/
    {
        DEBUG_OUT("警告:读取ID失败");
        return 0;
    }
    DEBUG_OUT("Get ID success，SENSOR ID is 0x%x", Sensor_IDCode);
    DEBUG_OUT("Config Register Number is %d ", cfgnum);
    if(Sensor_IDCode == OV7725_ID)
    {
        for( i = 0 ; i < cfgnum ; i++ )
        {
            if( 0 == SCCB_WriteByte(ov7727_reg[i].Address, ov7727_reg[i].Value) )
            {
                DEBUG_OUT("警告:写寄存器0x%x失败", ov7727_reg[i].Address);
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }
    DEBUG_OUT("OV7725 Register Config Success!");
    return 1;
}


