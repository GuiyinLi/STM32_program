#include "common.h"
#include "gpio.h"
#include "exti.h"
#include "spi.h"
#include "NRF24L0.h"

typedef enum
{
    NOT_INIT=0,
    TX_MODE,
    RX_MODE,
}nrf_mode_e;


typedef enum
{
    QUEUE_EMPTY = 0,        //队列空模式，只可入队列
    QUEUE_NORMAL,           //正常模式，可正常出入队列，即队列不空不满
    QUEUE_FULL,             //队列满模式，满了则不再添加，丢弃掉数据
}nrf_rx_queueflag_e; //中断接收时，队列状态标记位

#define NRF_CE_HIGH()	    PTE28_OUT=1
#define NRF_CE_LOW()	    PTE28_OUT=0			  //CE置低
#define NRF_PCSN_LOW()      PTA14_OUT=0
#define NRF_PCSN_HIGH()     PTA14_OUT=1
#define NRF_Read_IRQ()	    PTE27_IN

static  u8      NRF_WriteReg(u8 reg,u8 dat);
static  u8      NRF_ReadReg(u8 reg);

static  u8      NRF_ReadBuf(u8 reg,u8 *pBuf,u8 len);
static  u8      NRF_WriteBuf(u8 reg ,u8 *pBuf,u32 len);
static  u8      NRF_RW(u8 dat);
static  void    NRF_ISR_Tx_Handler(void);               //发送中断处理函数
static  void    NRF_ISR_Rx_Handler(void);
static  u8      NRF_Check(void);

volatile u8     nrf_mode=NOT_INIT;
volatile u8 *   isr_addr   =NULL;
volatile u32    isr_L      =0;     //L = len / MAX_ONCE_TX_NUM
volatile u8     isr_state  =0;

volatile u8     front = 0,rear = 0; //接收FIFO的指针
volatile u8     re_flag = QUEUE_EMPTY;

static  void    NRF_ISR_Tx_Dat_Once(void);



#define TX_ADR_WIDTH 	ADR_WIDTH  	//发射地址宽度
#define TX_PLOAD_WIDTH  MAX_ONCE_TX_NUM   //发射数据通道有效数据宽度0~32Byte 

#define RX_ADR_WIDTH    ADR_WIDTH
#define RX_PLOAD_WIDTH  MAX_ONCE_TX_NUM

u8 RX_BUF[RX_PLOAD_WIDTH];		//接收数据缓存
//u8 TX_BUF[TX_PLOAD_WIDTH];		//发射数据缓存

u8 RX_ISR_FIFO[RX_ISR_FIFO_PACKET][MAX_ONCE_TX_NUM];      //中断接收的FIFO

u8 TX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01};  // 定义一个静态发送地址
u8 RX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01};



#define CHANAL 40	//频道选择 

// SPI(nRF24L01) commands ,	NRF的SPI命令宏定义，详见NRF功能使用文档
#define NRF_READ_REG    0x00  // Define read command to register
#define NRF_WRITE_REG   0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses) ，NRF24L01 相关寄存器地址的宏定义
#define CONFIG      0x00  // 'Config' register address
#define EN_AA       0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR   0x02  // 'Enabled RX addresses' register address
#define SETUP_AW    0x03  // 'Setup address width' register address
#define SETUP_RETR  0x04  // 'Setup Auto. Retrans' register address
#define RF_CH       0x05  // 'RF channel' register address
#define RF_SETUP    0x06  // 'RF setup' register address
#define STATUS      0x07  // 'Status' register address
#define OBSERVE_TX  0x08  // 'Observe TX' register address
#define CD          0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0  0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1  0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2  0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3  0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4  0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5  0x0F  // 'RX address pipe5' register address
#define TX_ADDR     0x10  // 'TX address' register address
#define RX_PW_P0    0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1    0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2    0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3    0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4    0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5    0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17  // 'FIFO Status Register' register address


/*
 * 函数名：NRF_Init
 * 描述  ：SPI的 I/O配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void NRF_Init(void)
{
    //配置NRF管脚复用
    spi_init(NRF_SPI,MASTER);               //初始化SPI,主机模式
    
    gpio_init(PORTE,28, GPO,LOW);           //初始化CE，默认进入待机模式
    //gpio_init(PORTE,27, GPI,LOW);           //初始化IRQ管脚为输入 
    gpio_init(PORTA,14, GPO,HIGH);          //初始化PCSN管脚为输出，低电平选中  
    
#if IS_USE_ISR
    exti_init(PORTE,27, falling_up);        //初始化IRQ管脚为 :下降沿触发，内部上拉
#else
    gpio_init(PORTE,27, GPI,LOW);           //初始化IRQ管脚为输入     
#endif
    
    //配置NRF寄存器
    NRF_CE_LOW();	
    
    NRF_WriteReg(NRF_WRITE_REG+SETUP_AW,ADR_WIDTH - 2);     //设置地址长度为 TX_ADR_WIDTH
    
    NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);               //设置RF通道为CHANAL
 
    NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);              //设置TX发射参数,0db增益,2Mbps,低噪声增益开启 
    
    NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);                 //使能通道0的自动应答  
    
    NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);             //使能通道0的接收地址    
    
    //RX模式配置
    //NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址

    NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);    //选择通道0的有效数据宽度   
    
    //TX模式配置
    //NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //写TX节点地址 ,主要为了使能ACK   

    NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x0F);            //设置自动重发间隔时间:250us + 86us;最大自动重发次数:15次
    
#if IS_AUTO_RX_MODE
    NRF_RX_Mode();          //默认进入接收模式
#endif
    NRF_CE_HIGH();

    while(NRF_Check() == 0);                            //检测无线模块是否插入：如果卡在这里，说明没检测到无线模块
    
}


/*
 * 函数名：NRF_WriteReg
 * 描述  ：用于向NRF特定的寄存器写入数据
 * 输入  ：reg:NRF的命令+寄存器地址。
 		   dat:将要向寄存器写入的数据
 * 输出  ：NRF的status寄存器的状态
 * 调用  ：内部调用
 */
u8 NRF_WriteReg(u8 reg,u8 dat)
{
 	u8 status;
	 NRF_CE_LOW();
     
   	 /*置低CSN，使能SPI传输*/
    NRF_PCSN_LOW(); 
     
	/*发送命令及寄存器号 */
	status = NRF_RW(reg);
		 
	 /*向寄存器写入数据*/
    NRF_RW(dat); 
    
	/*CSN拉高，完成*/
	NRF_PCSN_HIGH();
    
	/*返回状态寄存器的值*/
   	return(status);
}


/*
 * 函数名：NRF_ReadReg
 * 描述  ：用于从NRF特定的寄存器读出数据
 * 输入  ：reg:NRF的命令+寄存器地址。
 * 输出  ：寄存器中的数据
 * 调用  ：内部调用
 */
u8 NRF_ReadReg(u8 reg)
{
 	u8 reg_val;

	NRF_CE_LOW();
    
   	 /*置低CSN，使能SPI传输*/
    NRF_PCSN_LOW(); 
				
  	 /*发送寄存器号*/
	NRF_RW(reg); 

	 /*读取寄存器的值 */
	reg_val = NRF_RW(NOP);
    
	/*CSN拉高，完成*/
	NRF_PCSN_HIGH();
    
	return reg_val;
}

/*
 * 函数名：NRF_WriteBuf
 * 描述  ：用于向NRF的寄存器中写入一串数据
 * 输入  ：reg:NRF的命令+寄存器地址。
 		   pBuf：存储了将要写入写寄存器数据的数组，外部定义
		   bytes: pBuf的数据长度	
 * 输出  ：NRF的status寄存器的状态
 * 调用  ：外部调用
 */
u8 NRF_WriteBuf(u8 reg ,u8 *pBuf,u32 len)
{
	 u8 status;
     
   	 /*置低CSN，使能SPI传输*/
    NRF_PCSN_LOW(); 
    
    /*发送寄存器号*/	
     status= NRF_RW(reg);      //continue,即不会取消片选

    /*向缓冲区写入数据*/
    spi_send(NRF_SPI,pBuf,len);	
    
  	/*CSN拉高，完成*/
	NRF_PCSN_HIGH();    
  	return (status);	//返回NRF24L01的状态 		
}


/*
 * 函数名：NRF_ReadBuf
 * 描述  ：用于从NRF的寄存器中读出一串数据
 * 输入  ：reg:NRF的命令+寄存器地址。
 		   pBuf：用于存储将被读出的寄存器数据的数组，外部定义
		   bytes: pBuf的数据长度	
 * 输出  ：NRF的status寄存器的状态
 * 调用  ：外部调用
 */
u8 NRF_ReadBuf(u8 reg,u8 *pBuf,u8 len)
{
 	u8 status, byte_cnt;
   // u8 time=0xff;
    NRF_CE_LOW();
    
   	 /*置低CSN，使能SPI传输*/
    NRF_PCSN_LOW(); 
    
    for(byte_cnt = 0 ;byte_cnt < len;byte_cnt++)
    {
        pBuf[byte_cnt] = NOP;
    }
    
	/*发送寄存器号*/	
	status = NRF_RW(reg); 

 	/*读取缓冲区数据*/
    spi_WR(NRF_SPI,pBuf,len);
    
	/*CSN拉高，完成*/
	NRF_PCSN_HIGH();
    
 	return status;		//返回寄存器状态值
}


/*
 * 函数名：NRF_RW
 * 描述  ：用于向NRF读/写一字节数据
 * 输入  ：写入的数据
 * 输出  ：读取得的数据
 * 调用  ：内部调用
 */
u8 NRF_RW(u8 dat)
{ 
    spi_WR(NRF_SPI,&dat,1);
    return  dat;
}

/*
 * 函数名：NRF_Check
 * 描述  ：主要用于NRF与MCU是否正常连接
 * 输入  ：无	
 * 输出  ：SUCCESS/ERROR 连接正常/连接失败
 * 调用  ：外部调用
 */
u8 NRF_Check(void)
{
#define NRF_CHECH_DATA  0xC2
	u8 buf[5]={NRF_CHECH_DATA,NRF_CHECH_DATA,NRF_CHECH_DATA,NRF_CHECH_DATA,NRF_CHECH_DATA};
	u8 buf1[5];
	u8 i; 

	/*写入5个字节的地址.  */ 
    NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);
    
	/*读出写入的地址 */
	NRF_ReadBuf(TX_ADDR,buf1,5); 
	
	/*比较*/
	for(i=0;i<5;i++)
	{
		if(buf1[i] != NRF_CHECH_DATA)
        {
            return 0 ;        //MCU与NRF不正常连接
        }
	} 
	return 1 ;             //MCU与NRF成功连接 	
}

//传输一次，数据最长为 32字节
static u8 NRF_TX_Dat_Once(u8 *txbuf)
{
	u8 state;

    /*ce为低，进入待机模式1*/
	NRF_CE_LOW();
    
	/*写数据到TX BUF 最大 32个字节*/						
    NRF_WriteBuf(WR_TX_PLOAD,txbuf,MAX_ONCE_TX_NUM);

    /*CE为高，txbuf非空，发送数据包 */   
    NRF_CE_HIGH();
	
	/*等待发送完成中断 */                            
	while(NRF_Read_IRQ()!=0);
	
	/*读取状态寄存器的值 */                              
	state = NRF_ReadReg(STATUS);

	 /*清除TX_DS或MAX_RT中断标志*/                  
	NRF_WriteReg(NRF_WRITE_REG+STATUS,state); 	

	NRF_WriteReg(FLUSH_TX,NOP);    //清除TX FIFO寄存器 

	 /*判断中断类型*/   
    return state;
}

 /*
 * 函数名：NRF_Tx_Dat
 * 描述  ：用于向NRF的发送缓冲区中写入数据
 * 输入  ：txBuf：存储了将要发送的数据的数组，外部定义	
 * 输出  ：发送结果，成功返回TXDS,失败返回MAXRT或ERROR
 * 调用  ：外部调用
 */ 
u8 NRF_Tx_Dat(u8 *txbuf,u32 len)
{
    u8 state;  
    u32 L;
    
    if( nrf_mode != TX_MODE)
    {
        NRF_TX_Mode();
    }
    
    L = (len+MAX_ONCE_TX_NUM-1) / MAX_ONCE_TX_NUM ; //进位取整
    
    while(L--)
    {
        state = NRF_TX_Dat_Once(txbuf);
        if( !(state & TX_DS))             //如果发送不成功，则结束
        {
            break;          //跳出while循环
        }
        txbuf += MAX_ONCE_TX_NUM;       //修正指向的位置
    }

#if IS_AUTO_RX_MODE     //自动进入接收模式
    NRF_RX_Mode();      
#endif
    
    return state;
} 

 /*
 * 函数名：NRF_Tx_Dat
 * 描述  ：用于向NRF的发送缓冲区中写入数据
 * 输入  ：txBuf：存储了将要发送的数据的数组，外部定义	
 * 输出  ：发送结果，成功返回TXDS,失败返回MAXRT或ERROR
 * 调用  ：外部调用
 */ 
void NRF_ISR_Tx_Dat(u8 *txbuf,u32 len)
{
    u32 L = (len+ MAX_ONCE_TX_NUM -1)/MAX_ONCE_TX_NUM;
    
    if( nrf_mode != TX_MODE)
    {
        NRF_TX_Mode();
    }
    
    while(isr_L != 0);          //等待isr_L 为 0 ，即 发送完毕
    
    isr_addr    = txbuf;
    isr_L       = L;
    
    NRF_ISR_Tx_Dat_Once();      //激活一次发送，此后的发送由中断来调用
} 


//由中断服务函数调用
void NRF_ISR_Tx_Handler(void)
{
    if(isr_addr == NULL)
    {
        return;         //无效
    }
    
    //检测发送结果                
	isr_state = NRF_ReadReg(STATUS);                    /*读取状态寄存器的值 */  

	NRF_WriteReg(NRF_WRITE_REG+STATUS,isr_state); 	    /*清除TX_DS或MAX_RT中断标志*/    

	NRF_WriteReg(FLUSH_TX,NOP);                         //清除TX FIFO寄存器 
    
    if( (!(isr_state & TX_DS) ) || (isr_L==0) )         //如果发送不成功,或者已经发送完成，则结束
    {
        isr_addr = NULL;                                //通过判断 isr_addr 就知道是否发送完毕。
        isr_L    = 0;                                   //isr_L就不一定了
        return;                                         //通过 isr_state 判断状态
    }
    
    //还没发送完成，就继续发送
    isr_addr += MAX_ONCE_TX_NUM;       //指向下一个地址
    
    NRF_ISR_Tx_Dat_Once();
}

void nrf_cpy( u8 * dst, u8 * src, u32 count)
{
      u32 n = (count + 7 ) / 8 ;
      switch (count % 8 ) {
      case 0 :   do {   * dst ++ = * src ++ ;
      case 7 :          * dst ++ = * src ++ ;
      case 6 :          * dst ++ = * src ++ ;
      case 5 :          * dst ++ = * src ++ ;
      case 4 :          * dst ++ = * src ++ ;
      case 3 :          * dst ++ = * src ++ ;
      case 2 :          * dst ++ = * src ++ ;
      case 1 :          * dst ++ = * src ++ ;
                } while ( -- n >    0 );
      }
}  

//中断发送函数，仅仅发送一次，就等待中断发送
void NRF_ISR_Tx_Dat_Once(void)
{
    if(isr_L > 0)
    {
        /*ce为低，进入待机模式1*/
        NRF_CE_LOW();

        /*写数据到TX BUF 最大 32个字节*/						
        NRF_WriteBuf(WR_TX_PLOAD,(u8 *)isr_addr,MAX_ONCE_TX_NUM);

        /*CE为高，txbuf非空，发送数据包 */   
        NRF_CE_HIGH();

        isr_L--;
    }
}

u8 NRF_ISR_Tx_State(void)
{
    if( nrf_mode != TX_MODE)
    {
        return TX_ISR_NULL;     //不是发送模式，没发送
    }
    
    if( isr_addr == NULL)
    {
        if(isr_state & TX_DS) 
        {
            return TX_ISR_SUCCEED;  //发送成功
        }
        
        if(isr_state != 0)
        {
            return TX_ISR_FAIL; //发送失败
        }
        
        return TX_ISR_NULL;     //没发送
    }
    else
    {
        return TX_ISR_SEND;     //发送中
    }
}

void NRF_Handler(void)
{
    if( nrf_mode == TX_MODE)
    {
        NRF_ISR_Tx_Handler();
    }
    else if( nrf_mode == RX_MODE )
    {
        NRF_ISR_Rx_Handler();
    }
}

 /*
 * 函数名：NRF_Rx_Dat
 * 描述  ：用于从NRF的接收缓冲区中读出数据
 * 输入  ：rxBuf：用于接收该数据的数组，外部定义	
 * 输出  ：接收结果，
 * 调用  ：外部调用
 */ 
u8 NRF_Rx_Dat(u8 *rxbuf)
{
	u8 state; 
    
    if( nrf_mode != RX_MODE)
    {
        NRF_RX_Mode();
    }
    
	NRF_CE_HIGH();	 //进入接收状态
	 /*等待接收中断*/
	while(NRF_Read_IRQ()!=0); 
	
	NRF_CE_LOW();  	 //进入待机状态
	/*读取status寄存器的值  */               
	state=NRF_ReadReg(STATUS);
	 
	/* 清除中断标志*/      
	NRF_WriteReg(NRF_WRITE_REG+STATUS,state);

	/*判断是否接收到数据*/
	if(state & RX_DR)                                 //接收到数据
	{
	    NRF_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
	    NRF_WriteReg(FLUSH_RX,NOP);                     //清除RX FIFO寄存器
	    return RX_DR; 
	}
	else
    {
		return ERROR;                    //没收到任何数据
    }
}

//由中断服务函数调用
void NRF_ISR_Rx_Handler(void)
{
    u8 state;
    
	NRF_CE_LOW();  	 //进入待机状态
    
	/*读取status寄存器的值  */               
	state=NRF_ReadReg(STATUS);
	 
	/* 清除中断标志*/      
	NRF_WriteReg(NRF_WRITE_REG+STATUS,state);
    
    if(re_flag == QUEUE_FULL)       //满了就直接清FIFO，退出
    {
        NRF_WriteReg(FLUSH_RX,NOP);                     //清除RX FIFO寄存器
        NRF_CE_HIGH();  	 //进入接收模式
        return;       //接收队列满了，不进行处理
    }
    
    //还没满，则继续接收

	/*判断是否接收到数据*/
	if(state & RX_DR)                                 //接收到数据
	{
	    NRF_ReadBuf(RD_RX_PLOAD,(u8 *)&(RX_ISR_FIFO[rear]),RX_PLOAD_WIDTH);  //读取数据
	    NRF_WriteReg(FLUSH_RX,NOP);                     //清除RX FIFO寄存器

        rear++;
        
        if(rear >= RX_ISR_FIFO_PACKET)
        {
            rear=0;                         //重头开始
        }
        
        if(rear == front)                   //追到屁股了，满了
        {
            re_flag = QUEUE_FULL;   
        }
        else
        {
            re_flag = QUEUE_NORMAL;
        } 
	}
	else    
    {
		                  //没收到任何数据
    }
    
    NRF_CE_HIGH();  	 //进入接收模式

}

void    NRF_ISR_Rx_rst()
{
    front = 0;
    rear =0;
    re_flag = QUEUE_EMPTY;
}

u8  NRF_ISR_Rx_Dat(u8 *rxbuf)
{
    
    if( nrf_mode != RX_MODE)
    {
        NRF_RX_Mode();
    }
    else
    {
        NRF_CE_HIGH();	 //进入接收状态
    }
    
    if(re_flag == QUEUE_EMPTY)   //空，直接返回0
    {
        return 0;
    }

    //复制数据
    nrf_cpy(rxbuf,(u8 *)&(RX_ISR_FIFO[front]),MAX_ONCE_TX_NUM);
    
    NRF_CE_LOW();  	 //进入待机状态
    
    front++;                //由于非空，所以可以直接出队列
    
    if(front >= RX_ISR_FIFO_PACKET)
    {
        front=0;            //重头开始
    }
    
    if(front == rear)       //追到屁股了，接收队列空
    {
        re_flag = QUEUE_EMPTY;
//        front=0;            //重头开始(可以省掉)
//        rear=0;
    }
    
    NRF_CE_HIGH();  	 //进入接收模式
    
    return RX_DR;
    
}

/*
 * 函数名：NRF_RX_Mode
 * 描述  ：配置并进入接收模式
 * 输入  ：无	
 * 输出  ：无
 * 调用  ：外部调用
 */
void NRF_RX_Mode(void)
{
	NRF_CE_LOW();	
    
    NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);             //使能通道0的自动应答  
    
    NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);         //使能通道0的接收地址 
    
    NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
    
    
    NRF_WriteReg(NRF_WRITE_REG+CONFIG, 0x0B | (IS_CRC16<<2));           //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 

#if 1
    /* 清除中断标志*/      
	NRF_WriteReg(NRF_WRITE_REG+STATUS,0xff);
    
    NRF_WriteReg(FLUSH_RX,NOP);                     //清除RX FIFO寄存器
#endif    
    
    /*CE拉高，进入接收模式*/	
    NRF_CE_HIGH();
    
    nrf_mode = RX_MODE;
}    

/*
 * 函数名：NRF_TX_Mode
 * 描述  ：配置发送模式
 * 输入  ：无	
 * 输出  ：无
 * 调用  ：外部调用
 */
void NRF_TX_Mode(void)
{  
    u32 i;

	NRF_CE_LOW();		
    
    NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //写TX节点地址 

    NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //设置RX节点地址 ,主要为了使能ACK   
    
    NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0A | (IS_CRC16<<2));    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发射模式,开启所有中断

    /*CE拉高，进入发送模式*/
    NRF_CE_HIGH();

    i=0xffff;
    while(i--);         //CE要拉高一段时间才进入发送模式
    //DELAY_MS(1); 

    nrf_mode = TX_MODE;
}
