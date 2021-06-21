/******************** 唐野岐士 ********************
 * 文件名       ：nrf24l01.c
 * 描述         ：nrf24l01芯片操作函数定义
 *
 * 实验平台     ：LQ K60开发板
 *                IRQ PTE25  CSN PTE26  MOSI PTD13   CE PTE28  SCK PTD12    MISO PTD14
 *
 * 作者          DAQI
**********************************************************************************/
#include "include.h"

const uint8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const uint8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址

//初始化24L01的IO口
void NRF24L01_Init(void)
{
    PORT_PCR_REG(PORTX[PORTE], 26) = PORT_PCR_MUX(1);
    GPIO_PDDR_REG(GPIOx[PORTE]) |= (1 <<26);    //  确定引脚为输出
    PORT_PCR_REG(PORTX[PORTE], 28) = PORT_PCR_MUX(1);
    GPIO_PDDR_REG(GPIOx[PORTE]) |= (1 <<28);    //  确定引脚为输出
    PORT_PCR_REG(PORTX[PORTE], 25) = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK ; // GPIO功能 , 内部上拉
    GPIO_PDDR_REG(GPIOx[PORTE]) &= ~(1 <<25);    //  确定引脚为输入

    spi_init(MASTER);   //初始化SPI
	
    Clr_NRF24L01_CE;	 //使能24L01  NRF24L01_CE
    Set_NRF24L01_CSN;    //SPI片选取消 NRF24L01_CSN	 		  
}

//检测24L01是否存在
//返回值:0，成功;1，失败	
uint8 NRF24L01_Check(void)
{
	
  uint8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
  uint8 i;
	   	 
  NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
  NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
  for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
  if(i!=5)return 1;//检测24L01错误	
  return 0;		 //检测到24L01
}	
//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
uint8 NRF24L01_Write_Reg(uint8 reg,uint8 value)
{
    uint8 status;	
    Clr_NRF24L01_CSN;                 //使能SPI传输
    status =SPIx_ReadWriteByte(reg);//发送寄存器号 
    SPIx_ReadWriteByte(value);      //写入寄存器的值
    Set_NRF24L01_CSN;                 //禁止SPI传输	   
    return(status);       			//返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
uint8 NRF24L01_Read_Reg(uint8 reg)
{
    u8 reg_val;	    
    Clr_NRF24L01_CSN;          //使能SPI传输		
    SPIx_ReadWriteByte(reg);   //发送寄存器号
    reg_val=SPIx_ReadWriteByte(0XFF);//读取寄存器内容
    Set_NRF24L01_CSN;          //禁止SPI传输		    
    return(reg_val);           //返回状态值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
uint8 NRF24L01_Read_Buf(uint8 reg,uint8 *pBuf,uint8 len)
{
    uint8 status,u8_ctr;	       
    Clr_NRF24L01_CSN;           //使能SPI传输
    status=SPIx_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
    for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPIx_ReadWriteByte(0XFF);//读出数据
    Set_NRF24L01_CSN;       //关闭SPI传输
    return status;        //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
uint8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
    uint8 status,u8_ctr;	    
    Clr_NRF24L01_CSN;          //使能SPI传输
    status = SPIx_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
    for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPIx_ReadWriteByte(*pBuf++); //写入数据	 
    Set_NRF24L01_CSN;       //关闭SPI传输
    return status;          //返回读到的状态值
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
uint8 NRF24L01_TxPacket(uint8 *txbuf)
{
     uint8 sta;  
     Clr_NRF24L01_CE;
     NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
     Set_NRF24L01_CE;//启动发送	   
     while(NRF24L01_IRQ!=0);//等待发送完成
     sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
     NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
     if(sta&MAX_TX)//达到最大重发次数
     {
	NRF24L01_Write_Reg(NRF24L01_FLUSH_TX,0xff);//清除TX FIFO寄存器 
	return MAX_TX; 
     }
    if(sta&TX_OK)//发送完成
    {
	return TX_OK;
    }
    return 0xff;//其他原因发送失败
}
//启动NRF24L01读取一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
uint8 NRF24L01_RxPacket(u8 *rxbuf)
{
    u8 sta;		    							      
    sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
   if(sta&RX_OK)//接收到数据
   {
       NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
       NRF24L01_Write_Reg(NRF24L01_FLUSH_RX,0xff);//清除RX FIFO寄存器 
       return 0; 
    }	   
    return 1;//没收到任何数据
}	

//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void RX_Mode(void)
{
    exti_init(PORTE, 25, falling_up);
    delay_2us();
    
    Clr_NRF24L01_CE;	  
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,40);	     //设置RF通信频率		  
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
    Set_NRF24L01_CE; //CE为高,进入接收模式 
}
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void TX_Mode(void)
{	
//    PORT_PCR_REG(PORTX[PORTE], 25) = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK ; // GPIO功能 , 内部上拉
//    GPIO_PDDR_REG(GPIOx[PORTE]) &= ~(1 <<25);    //  确定引脚为输入
//    disable_irq(PORTE+87);
//    delay_2us();
    
    Clr_NRF24L01_CE;	    
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
	
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,40);       //设置RF通道为40
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
    Set_NRF24L01_CE;//CE为高,10us后启动发送
}		  
							 




