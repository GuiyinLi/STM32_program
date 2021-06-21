/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：NRF24L0.h
 * 描述         ：NRF24L0函数库
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：野火嵌入式开发工作室
 * 淘宝店       ：http://firestm32.taobao.com
 * 技术支持论坛 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/
#ifndef _NRF24L0_H_
#define _NRF24L0_H_     1

#include "spi.h"



typedef enum
{
    TX_ISR_NULL,    //没发送
    TX_ISR_SEND,    //发送中
    TX_ISR_SUCCEED, //发送成功
    TX_ISR_FAIL,    //发送失败
}tx_isr_state_e;

#define NRF_CFG     SPI_CFG
#define NRF_SPI     SPI0

//以下是用户配置的选项

#define MAX_ONCE_TX_NUM     32      //一次传输最大可支持的字节数（0~32）
#define ADR_WIDTH           5       //定义地址长度（3~5）
#define IS_CRC16            1       //1表示使用 CRC16，0表示 使用CRC8 (0~1)
#define IS_AUTO_RX_MODE     1       //1表示发送完成大小后，会自动进入接收模式（发送的时候会自动进入发送模式）。
                                    //0表示不需要自动进入接收模式

//中断接收和发送的配置
#define IS_USE_ISR          1      //是否使用ISR中断发送和接收。0：不使用，1：使用
#define RX_ISR_FIFO_PACKET  (20)    //中断接收的缓冲区可接收包的数目（包的大小由 MAX_ONCE_TX_NUM 决定）。一副图像的大小为80*60/8 = 18.75*32
//#define TX_ISR_NUM          10      //中断发送的缓冲区能同时有多少个发送请求.超过了则不处理。


//配置到这里结束


#define MAX_RT              0x10    //达到最大重发次数中断标志位
#define TX_DS		        0x20    //发送完成中断标志位	  // 
#define RX_DR		        0x40    //接收到数据中断标志位


//#define NRF_PACKET_SIZE     (  1  +  ADR_WIDTH  +  9  +  MAX_ONCE_TX_NUM  + IS_CRC16  )




//中断发送部分
extern  u8 RX_ISR_FIFO[RX_ISR_FIFO_PACKET][MAX_ONCE_TX_NUM];






extern  void NRF_Init(void);
//extern  u8 NRF_Check(void);
extern  u8 NRF_Tx_Dat(u8 *txbuf,u32 len);               //len 必须是 MAX_ONCE_TX_NUM 整数倍，剩下不足32位的，数据无效
extern  u8 NRF_Rx_Dat(u8 *rxbuf);                       //每次读取 MAX_ONCE_TX_NUM 个字节（返回接收结果：成功或者失败）




extern  void    NRF_Handler(void);
extern  void    NRF_ISR_Tx_Dat(u8 *txbuf,u32 len);      //中断方式发送数据
extern  u8      NRF_ISR_Tx_State(void);                 //查询发送状态

extern  u8      NRF_ISR_Rx_Dat(u8 *rxbuf);              //每次读取 MAX_ONCE_TX_NUM 个字节（返回接收结果：成功或者失败，成功了，还有不断检查是否有数据接收到）




//发送和接收的时候，会自动设置对应的模式。
//通常情况下不需要我们手动设置
extern  void NRF_RX_Mode(void);
extern  void NRF_TX_Mode(void);

#endif      //_NRF24L0_H_