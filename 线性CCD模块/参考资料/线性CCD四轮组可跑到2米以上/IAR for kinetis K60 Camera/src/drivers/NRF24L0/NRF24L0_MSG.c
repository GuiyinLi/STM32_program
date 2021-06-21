#include "common.h"
#include "gpio.h"
#include "exti.h"
#include "spi.h"
#include "ov7725.h"
#include "NRF24L0.h"
#include "NRF24L0_MSG.h"

u32 rxbuflen = 0;           //用于接收方返回接收到多少数据。（包含第一次传递进去的那个包大小）



static nrf_result_e  tx_com_img(u8 * sendbuf);
static nrf_result_e  tx_com_event_query(u8 * sendbuf);

static nrf_result_e  rx_com_img(u8 * sendbuf);
static nrf_result_e  rx_com_event_query(u8 * sendbuf);


typedef	nrf_result_e (* NRF_PFUNC)(u8 *);               //定义函数指针，用于保存命令控制的

//控制命令执行函数结构体
typedef	struct
{
	NRF_PFUNC	img_func;                   //传输图像
	NRF_PFUNC	event_query;                //事件查询
}com_func_t;



//Receive Command Vector Table  液晶控制部分处理函数
NRF_PFUNC rx_com_table[COM_MAX] =
{	
    rx_com_img,	
    rx_com_event_query
};


//Transfer Command Vector Table 摄像头部分处理函数
NRF_PFUNC tx_com_table[COM_MAX] =
{
	tx_com_img,
	tx_com_event_query
};

//发送命令。前两个字节用在命令识别，后面就是需要传输的内容
nrf_result_e NRF_MSG_send(crtl_e  com,u8 * sendbuf)
{
    nrf_result_e result;
    ASSERT(com < COM_MAX);          //命令不能超过最大的数目
    
    //把前两个写进数据缓冲区
    sendbuf[0]= com;
    sendbuf[1]= ~com;
    
    //执行对应的发送函数
    result = (tx_com_table[(u8)com] )(sendbuf);  //对应的发送函数要注意前面两个字节不能用
    
    return result;
}

//发送方命令对应的函数

//发送图像
nrf_result_e  tx_com_img(u8 * sendbuf)
{
    //因为直接把图像采集缓冲区，这里不需要负责图像，仅仅发送数据就好
    sendbuf[CAMERA_SIZE + 2*COM_LEN -2]=(u8)~COM_IMG;         //在尾部写入结束传输标记
    sendbuf[CAMERA_SIZE + 2*COM_LEN -1]=(u8)COM_IMG;
    
    NRF_ISR_Tx_Dat(sendbuf, CAMERA_SIZE + 2*COM_LEN );      //中断方式发送数据  
    
    return NRF_RESULT_NULL;
}

//查询事件，暂不用
nrf_result_e  tx_com_event_query(u8 * sendbuf)
{
    return NRF_RESULT_NULL;
}



//接收方

//查询是否有接收数据，并进行处理。sendbuf 为可供使用的缓冲区
nrf_result_e NRF_MSG_receive(crtl_e  *com,u8 * rebuf)
{
    nrf_result_e result;
    u8  status;	//用于判断接收/发送状态
    u8  flag = 0;   //但接收到数据有误，就标记为1
    u8  rxflag  = 0;//标记是否有接收到数据
    crtl_e  comtmp;
    do
    {
        flag = 0;
        status = NRF_ISR_Rx_Dat(rebuf); //中断接收数据
        if(status != RX_DR) 
        {
            //没有接收到数据
            if(rxflag)
            {
                //有接收到数据，但接收数据无效
                return NRF_RESULT_RX_NOVALID;
            }
            else
            {
                return NRF_RESULT_RX_NO;
            }
        }
        rxflag = 1;
        comtmp = rebuf[0];
        if((comtmp < COM_MAX) && (rebuf[1] ==  (u8)~comtmp) )
        {
            flag = 1;
        }
    }while(flag==0);           //当com无误的时候，才往下执行
    
    *com = comtmp;

    //执行对应的发送函数
    result = ((rx_com_table[comtmp]))(rebuf);  //对应的发送函数要注意前面两个字节不能用
    
    return result;
}

//接收图像
nrf_result_e rx_com_img(u8 * rebuf)
{
    //因为是识别对应的命令后才执行，需要把图像采集完整才退出
    //由于第一包已经接收到，才会调用此函数，我们需要继续采集剩余的图像
    
    u32 len = (CAMERA_SIZE + 2*COM_LEN -1)/MAX_ONCE_TX_NUM ;
    u8  status;	//用于判断接收/发送状态
    u8  *buftemp = rebuf;
    
    rxbuflen = CAMERA_SIZE + 2*COM_LEN; //通知顶层接收到数据长度
    
    do
    {
        rebuf += MAX_ONCE_TX_NUM;         //移动到下一个包（传递进来的时候没扣掉那个包）
        
        //需要读取 len 个包保存在 rebuf + MAX_ONCE_TX_NUM 地址缓冲区后面
        do{
            status = NRF_ISR_Rx_Dat(rebuf); //中断接收数据
        }while(status != RX_DR);                //等待接收到数据
        
        len--;
        
    }while(len);
    
    //校验尾部数据是否正确
    if( 
            (buftemp[CAMERA_SIZE + 2*COM_LEN -2] ==   (u8)~COM_IMG)
     &&     (buftemp[CAMERA_SIZE + 2*COM_LEN -1] ==   (u8)COM_IMG)        )
    {
        return NRF_RESULT_RX_VALID;
    }
    
    return NRF_RESULT_RX_NOVALID;
    
}

//事件查询
nrf_result_e rx_com_event_query(u8 * rebuf)
{
    
    return NRF_RESULT_NULL;
}