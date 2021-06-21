#include "common.h"
#include "spi.h"
#include "lptmr.h"

volatile struct SPI_MemMap *SPIN[3]={SPI0_BASE_PTR,SPI1_BASE_PTR,SPI2_BASE_PTR}; //定义三个指针数组保存 SPIx 的地址


//=========================================================================
//函数名称：spi_init
//函数参数：spin：SPI通道号。
//          Master:是否是主机。
//函数返回：无
//功能概要：SPI初始化。
//=========================================================================
void spi_init(SPIn spin,SPI_CFG master)
{
    
    //使能SPI模块时钟，配置SPI引脚功能
    if(spin == 0)
    {
        SIM_SCGC6 |= SIM_SCGC6_DSPI0_MASK;
        //PORTA_PCR14 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//PCS0   //DSE=1:输出时高驱动能力
        PORTA_PCR15 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//SCK
        PORTA_PCR16 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//SOUT
        PORTA_PCR17 = 0 | PORT_PCR_MUX(0x2);//SIN
    }
    else if(spin == 1)
    {
        SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;
        PORTE_PCR1 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK; //SOUT
        PORTE_PCR2 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK; //SCK
        PORTE_PCR3 = 0 | PORT_PCR_MUX(0x2);                     //SIN
        PORTE_PCR4 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK; //PCS0
    }
    else 
    {   
        SIM_SCGC3 |= SIM_SCGC3_SPI2_MASK;
        PORTD_PCR13 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK; //SOUT
        PORTD_PCR12 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK; //SCK
        PORTD_PCR14 = 0 | PORT_PCR_MUX(0x2)/* | PORT_PCR_DSE_MASK*/;                     //SIN
        //PORTD_PCR15 = 0 | PORT_PCR_MUX(0x2)/* | PORT_PCR_DSE_MASK*/; //PCS1     
    }
    
    SPI_MCR_REG(SPIN[spin]) = 0 
                 | SPI_MCR_CLR_TXF_MASK     //Clear the Tx FIFO counter.
                 | SPI_MCR_CLR_RXF_MASK     //Clear the Rx FIFO counter.
                 //| SPI_MCR_PCSIS_MASK
                 | SPI_MCR_HALT_MASK;

    
    //根据主从机模式设置工作模式。MCU提供最大主机频率是1/2主频，最大从机频率是1/4主频
    if(master == MASTER)
    {
        SPI_MCR_REG(SPIN[spin]) =  (0
                                    |  SPI_MCR_MSTR_MASK //Master,主机模式
                                   // |   SPI_MCR_PCSIS(2) //PCS1
                                    );      
        SPI_CTAR_REG(SPIN[spin],0) = (0
                       //| SPI_CTAR_DBR_MASK          //双波特率 ，这里设 DBR=1，CPHA=1，PBR=00，得SCK Duty Cycle 为 50/50
                       //| SPI_CTAR_CPHA_MASK         //数据在SCK上升沿改变（输出），在下降沿被捕捉（输入读取）。如果是0，则反之。  w25x16在上升沿读取数据；NRF24L01在上升沿读取数据
                        | SPI_CTAR_PBR(1)            //波特率分频器 ，0~3 对应的分频值Prescaler为 2、3、5、7
                         
                        | SPI_CTAR_PDT(0x00)         //延时因子为 PDT*2+1 ，这里PDT为3，即延时因子为7。PDT为2bit
                        | SPI_CTAR_BR(0)             //波特率计数器值 ,当BR<=3,分频Scaler 为 2*（BR+1） ，当BR>=3，分频Scaler 为 2^BR  。BR为4bit
                                                    //SCK 波特率 = (fSYS/Prescaler) x [(1+DBR)/Scaler ]          fSYS 为 Bus clock
                                                    //              100M  2          1    2   = 50M   这里以最大的来算
                                                    //              100M   5          1    2   =20M
                         
                       //| SPI_CTAR_CPOL_MASK         //时钟极性，1表示 SCK 不活跃状态为高电平,   NRF24L01 不活跃为低电平
                        | SPI_CTAR_FMSZ(0x07)        //每帧传输 7bit+1 ，即8bit （FMSZ默认就是8）  
                      // | SPI_CTAR_LSBFE_MASK        //1为低位在前。
                        //| SPI_CTAR_CSSCK(1)          //
                        //|SPI_CTAR_PCSSCK(2)         //设置片选信号有效到时钟第一个边沿出现的延时的预分频值。tcsc延时预分频 2*x+1；
                       ); 
                        //LSBFE 为 0 ，数据在前
    }
    else
    {
        //默认从机模式
        SPI_CTAR_SLAVE_REG(SPIN[spin],0) = 0 
                            | SPI_CTAR_SLAVE_FMSZ(0x07)
                            | SPI_CTAR_SLAVE_CPOL_MASK 
                            | SPI_CTAR_SLAVE_CPHA_MASK; 
   }
    //DELAY_MS(100);
   /*************  清标志位  ***************/
    SPI_SR_REG(SPIN[spin]) = (SPI_SR_EOQF_MASK     //End of Queue Flag，发送队列空了，发送完毕
                         | SPI_SR_TFUF_MASK     //Transmit FIFO Underflow Flag，传输FIFO下溢标志位，SPI为从机模式，Tx FIFO为空，而外部SPI主机模式启动传输，标志位就会置1，写1清0
                         | SPI_SR_TFFF_MASK     //Transmit FIFO Fill Flag，传输FIFO满标志位。 写1或者DMA控制器发现传输FIFO满了就会清0。 0表示Tx FIFO满了
                         | SPI_SR_RFOF_MASK     //Receive FIFO Overflow Flag，接收FIFO溢出标志位。
                         | SPI_SR_RFDF_MASK);   //Receive FIFO Drain Flag，接收FIFO损耗标志位，写1或者DMA控制器发现传输FIFO空了就会清0。0表示Rx FIFO空
    
    
    SPI_MCR_REG(SPIN[spin]) &= ~SPI_MCR_HALT_MASK;     //启动SPI传输。1为暂停，0为启动
    
    DELAY_MS(1);
}


//=========================================================================
//函数名称：spi_send
//函数参数：spin：SPI通道号。
//          data[]：需要发送的数据。
//          len：数据长度。
//函数返回：无
//功能概要：SPI发送数据。
//=========================================================================
void spi_send(SPIn spin,u8 data[],u32 len)
{
    u32 i = 0;
    u8 temp;
    SPI_TX_WAIT(spin);

    do
    {
        /*************  清标志位  ***************/
        SPI_SR_REG(SPIN[spin]) = (SPI_SR_EOQF_MASK
                                | SPI_SR_TFUF_MASK
                                | SPI_SR_TFFF_MASK
                                | SPI_SR_RFOF_MASK
                                | SPI_SR_RFDF_MASK
                                );
    
        /************** 清FIFO计数器 **************/
        SPI_MCR_REG(SPIN[spin])    |=  (SPI_MCR_CLR_TXF_MASK     //Clear TX FIFO.写1清 Tx FIFO counter
                                   |SPI_MCR_CLR_RXF_MASK     //Clear RX FIFO. 写1清 the Rx FIFO counter.
                                   );
        //SPI_SR_REG(SPIN[spin]) |= SPI_SR_RFDF_MASK;
    }while( (SPI_SR_REG(SPIN[spin]) & SPI_SR_RFDF_MASK));   //如果 Rx FIFO 非空，则清FIFO.

    /***************** 发送len-1个数据 *******************/                                                ;
    for(i = 0;i < (len-1);i++)
    {
        //DELAY_MS(1); 
        SPI_PUSHR_REG(SPIN[spin]) = 0 
                                | SPI_PUSHR_CONT_MASK   //Continuous Peripheral Chip Select Enable，1为 传输期间保持PCSn信号 ，即继续传输数据
                                | SPI_PUSHR_CTAS(0)
                                | SPI_PUSHR_TXDATA(data[i]);    //要传输的数据
        
        while( !(SPI_SR_REG(SPIN[spin]) & SPI_SR_RFDF_MASK));    //RFDF为1，Rx FIFO is not empty.
        temp = (u8)SPI_POPR_REG(SPIN[spin]);    //读取一次接收的数据    
        SPI_SR_REG(SPIN[spin]) |= SPI_SR_RFDF_MASK;
    }

    /***************** 发送最后一个数据 *******************/    
    SPI_PUSHR_REG(SPIN[spin]) = 0 
                   | SPI_PUSHR_CTAS(0)
                   | SPI_PUSHR_EOQ_MASK         //End Of Queue，1为 传输SPI最后的数据
                   | SPI_PUSHR_TXDATA(data[i]); 
    
    SPI_EOQF_WAIT(spin);    //要及时把RX FIFO的东西清掉，不然这里就无限等待
    
    while( !(SPI_SR_REG(SPIN[spin]) & SPI_SR_RFDF_MASK));    //RFDF为1，Rx FIFO is not empty.
    temp = (u8)SPI_POPR_REG(SPIN[spin]);    //读取一次接收的数据          
    //SPI_SR_REG(SPIN[spin]) |= SPI_SR_RFDF_MASK;
    

}

//=========================================================================
//函数名称：spi_send
//函数参数：spin：SPI通道号。
//          data[]：需要发送的数据。
//          len：数据长度。
//函数返回：无
//功能概要：SPI发送数据。
//=========================================================================
void spi_WR(SPIn spin,u8* data,u32 len)
{
    u32 i = 0;

    SPI_TX_WAIT(spin);
    
    do
    {
        /*************  清标志位  ***************/
        SPI_SR_REG(SPIN[spin]) = (SPI_SR_EOQF_MASK
                                | SPI_SR_TFUF_MASK
                                | SPI_SR_TFFF_MASK
                                | SPI_SR_RFOF_MASK
                                | SPI_SR_RFDF_MASK
                                );
    
        /************** 清FIFO计数器 **************/
        SPI_MCR_REG(SPIN[spin])    |=  (SPI_MCR_CLR_TXF_MASK     //Clear TX FIFO.写1清 Tx FIFO counter
                                   |SPI_MCR_CLR_RXF_MASK     //Clear RX FIFO. 写1清 the Rx FIFO counter.
                                   );        
    }while( (SPI_SR_REG(SPIN[spin]) & SPI_SR_RFDF_MASK));   //如果 Rx FIFO 非空，则清FIFO.
    
    
    /***************** 发送len-1个数据 *******************/  
    for(i = 0;i < (len-1);i++)
    {
        SPI_PUSHR_REG(SPIN[spin]) = 0 
                                | SPI_PUSHR_CONT_MASK   //Continuous Peripheral Chip Select Enable，1为 传输期间保持PCSn信号 ，即继续传输数据
                                | SPI_PUSHR_CTAS(0)
                                | SPI_PUSHR_TXDATA(data[i]);    //要传输的数据
        
        while( !(SPI_SR_REG(SPIN[spin]) & SPI_SR_RFDF_MASK));    //RFDF为1，Rx FIFO is not empty.
        data[i] = (u8)SPI_POPR_REG(SPIN[spin]);    //保存接收到的数据 
        SPI_SR_REG(SPIN[spin]) |= SPI_SR_RFDF_MASK;
    }
    
    /***************** 发送最后一个数据 *******************/    
    SPI_PUSHR_REG(SPIN[spin]) = 0 
                   | SPI_PUSHR_CTAS(0)
                   | SPI_PUSHR_EOQ_MASK         //End Of Queue，1为 传输SPI最后的数据
                   | SPI_PUSHR_TXDATA(data[i]); 
    
    while( !(SPI_SR_REG(SPIN[spin]) & SPI_SR_RFDF_MASK));    //RFDF为1，Rx FIFO is not empty.
    data[i] = (u8)SPI_POPR_REG(SPIN[spin]);    //保存接收到的数据            

    SPI_EOQF_WAIT(spin);        //要及时把RX FIFO的东西清掉，不然这里就无限等待
}

//=========================================================================
//函数名称：spi_re
//函数参数：spin：SPI通道号。
//          data[]：需要接收的数据。
//函数返回：返回接收到字节的长度
//功能概要：SPI接收数据。
//=========================================================================
u32 spi_re_max(SPIn spin,u8 data[],u32 maxlen)
{
    u32   n=0;

    while(SPI_SR_REG(SPIN[spin]) & SPI_SR_RFDF_MASK)    //RFDF为1，Rx FIFO is not empty.
    {
        data[n++] = (u8)SPI_POPR_REG(SPIN[spin]);    //保存接收到的数据            
        SPI_SR_REG(SPIN[spin]) |= SPI_SR_RFDF_MASK;
        if(n>=maxlen)
        {
            break;
        }
    }
#if 1      
    /*************  清标志位  ***************/
    SPI_SR_REG(SPIN[spin]) = (  SPI_SR_EOQF_MASK
                               | SPI_SR_TFUF_MASK
                               | SPI_SR_TFFF_MASK
                               | SPI_SR_RFOF_MASK
                               | SPI_SR_RFDF_MASK
                               );
    
    /************** 清FIFO计数器 **************/
    SPI_MCR_REG(SPIN[spin])  |=  (SPI_MCR_CLR_TXF_MASK     //Clear the Tx FIFO counter.
                                |SPI_MCR_CLR_RXF_MASK     //Clear the Rx FIFO counter.
                                );
#endif
    return n; //n为0，则没接收到数据
}

//=========================================================================
//函数名称：spi_re
//函数参数：spin：SPI通道号。
//          data[]：需要接收的数据。
//函数返回：返回接收到字节的长度
//功能概要：SPI接收数据。
//=========================================================================
u32 spi_re(SPIn spin,u8 data[])
{
    u32   n=0;

    while(SPI_SR_REG(SPIN[spin]) & SPI_SR_RFDF_MASK)    //RFDF为1，Rx FIFO is not empty.
    {
        data[n++] = (u8)SPI_POPR_REG(SPIN[spin]);    //保存接收到的数据            
        SPI_SR_REG(SPIN[spin]) |= SPI_SR_RFDF_MASK;
    }
     
    /*************  清标志位  ***************/
    SPI_SR_REG(SPIN[spin]) = (SPI_SR_EOQF_MASK
                       | SPI_SR_TFUF_MASK
                       | SPI_SR_TFFF_MASK
                       | SPI_SR_RFOF_MASK
                       | SPI_SR_RFDF_MASK
                       );
    
    /************** 清FIFO计数器 **************/
    SPI_MCR_REG(SPIN[spin])  |=  (SPI_MCR_CLR_TXF_MASK     //Clear the Tx FIFO counter.
                                |SPI_MCR_CLR_RXF_MASK     //Clear the Rx FIFO counter.
                                );

    return n; //n为0，则没接收到数据
}
    

/*************************************************************************
*                             野火嵌入式开发工作室
*  
*  函数名称：spi_irq_EN
*  功能说明：开SPI中断
*  参数说明：SPIn       模块号（SPI0~SPI2）
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
void spi_irq_EN(SPIn spin)
{
  SPI_RSER_REG(SPIN[spin]) |=SPI_RSER_RFDF_RE_MASK;     //开启SPI接收中断 
  enable_irq(spin+26);			                //开接收引脚的IRQ中断
}

/*************************************************************************
*                             野火嵌入式开发工作室
*  
*  函数名称：spi_irq_DIS
*  功能说明：开串口接收中断
*  参数说明：UARTn       模块号（UART0~UART5）
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
void spi_irq_DIS(SPIn spin)
{
  SPI_RSER_REG(SPIN[spin]) &=~SPI_RSER_RFDF_RE_MASK;    //关闭SPI接收中断  
  disable_irq(spin+26);			                //开接收引脚的IRQ中断
}