#include "can.h"
#include "led.h"
#include "usart2.h"
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((6+7+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	CAN_InitTypeDef        CAN_InitStructuure;
	GPIO_InitTypeDef       GPIO_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
  #endif
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);    //使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;     //复用推挽  PA11--RX PA12--TX
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_11;   
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11|GPIO_PinSource12,GPIO_AF_CAN1);
	
	//CAN单元设置
	CAN_InitStructuure.CAN_ABOM=DISABLE;                 //软件自动离线管理
	CAN_InitStructuure.CAN_AWUM=DISABLE;                 //睡眠模式通过软件唤醒
	CAN_InitStructuure.CAN_Mode=mode;                    //0:普通模式 1:循环模式
	CAN_InitStructuure.CAN_NART=ENABLE;                  //禁止报文自动传送
	CAN_InitStructuure.CAN_RFLM=DISABLE;                 //报文不锁定 新的覆盖旧的
	CAN_InitStructuure.CAN_TTCM=DISABLE;                 //非时间触发模式
	CAN_InitStructuure.CAN_TXFP=DISABLE;                 //优先级由报文标志符决定
	//设置波特率
	CAN_InitStructuure.CAN_BS1=tbs1;                     //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructuure.CAN_BS2=	tbs2;                    //Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructuure.CAN_Prescaler=brp;                //分频系数(Fdiv)为brp+1	
	CAN_InitStructuure.CAN_SJW=tsjw;                     //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	
	CAN_Init(CAN1,&CAN_InitStructuure);                   //初始化CAN1
	
	CAN_FilterInitStructure.CAN_FilterNumber=0;                         //过滤器组0 根据位宽的不同 每个过滤器组可提供1个32位过滤器 或者 两个16位过滤器
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;       //屏蔽位模式   FBM0=0  
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;      //位宽 32  FSC0=1
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;                    //32为ID   标识符模式下的ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x036F;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;                //32位MASK  屏蔽位模式下的ID
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x01F8;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;  //分配到FIFO 0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                //激活筛选器
	/*每个过滤器组有两个寄存器 分别是标志符寄存器和屏蔽位寄存器
	  屏蔽寄存器中设置为高的值所对应的标志符寄存器的位为必须匹配位
		其实 寄存器里面的ID就是用于筛选的ID 而寄存器里面的屏蔽（掩码）只不过是在匹配的过程中 加了限制条件
		这里的限制条件就是指ID中那些位是必须匹配位 那些位是不必关心位
		在16位模式下两个32位寄存器被分成了四个16位寄存器 在屏蔽模式下 高16位为掩码 低16位为ID
		二者的关系和32Bit的相同，只不过数据的宽度发生了变化
		
		32Bit的屏蔽位模式下 两个寄存器分别为标志符寄存器和屏蔽位寄存器
		32Bit的列表模式下 两个寄存器均为标志符寄存器 只要接收到的ID和
											其中一个寄存器的值完全相同即可接受该设备发送的数据
		16bit的屏蔽位模式下 两个寄存器的低八位分别起到32bit中标志符的作用 而高八位起到32Bit中屏蔽位的作用
		                    只要结合到的数据符合其中一个寄存器的要求 就可以接收数据
		16Bit的列表模式下 两个32位寄存器分成4个16位的寄存器 只要接收到的ID和4个中的一个值完全匹配 便可接收数据
		*/
		/*例如32位屏蔽模式      List:FFFF 0000
		                        Mask:FF00 FF00     //意思是接收到的ID格式必须为FFxx00xx
				  32位列表模式      List:FFFF 0000
					                  Mask:FF00 FF00     //意思是接收到的ID可以是FFFF 0000也可以是FF00 FF00
				  16位屏蔽模式      List:FFFF 0FF0
		                        Mask:FF00 FF00     //意思是接收到的ID格式必须为xFFx或者FFxx
					16位列表模式      List:FFFF 0FF0
		                        Mask:FF00 FF00     //意思是接收到的ID可以是 FFFF/0FF0/FF00/FF00
		*/
		/*ID的第一个作用（用于CAN的仲裁） ID值越低 报文优先级越高
		  ID的第二个作用（配合筛选器） 设定ID的接收范围 过滤掉不需要的ID信息
		*/
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	#if CAN_RX0_INT_ENABLE
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);                              //消息挂号中断允许
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  #endif
	return 0;
}

#if CAN_RX0_INT_ENABLE   //使能RX0中断
//中断服务函数
void CAN1_RX0_IRQHandler(void)
{
	int i=0;
	CanRxMsg RxMessage;
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)==SET)
	{
	  CAN_Receive(CAN1,0,&RxMessage);    //存储接收到得信息
		for(i=0;i<len;i++)                 //len<=8
		{
			USART_SendData(USART2,RxMessage.Data[i]);		
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束		该句不能省！！		 
		}
		USART_SendData(USART2,len);		
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束		该句不能省！！	
		LED=!LED;
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	}
}
#endif

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 Can_Send_Msg(u8* msg,u8 len)
{
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	//仲裁段
	TxMessage.ExtId=0x00;                     //扩展标识符   0x168
	TxMessage.StdId=0x00;                     //标准标识符
	TxMessage.RTR=CAN_RTR_Data;               //数据帧
	//控制段  IDE DLC r0         r1 r0 DLC
	TxMessage.IDE=CAN_Id_Standard;            //标准帧
	TxMessage.DLC=len;                        //数据长度
	//数据段
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];
	mbox=CAN_Transmit(CAN1,&TxMessage);
	i=0;
	while((CAN_TransmitStatus(CAN1,mbox)==CAN_TxStatus_Failed)&&(i<0xfff)) i++;     //等待发送结束
	if(i>=0xfff) return 1;   //发送失败
	return 0;                //发送成功
}

//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 Can_Receive_Msg(u8 *buf)
{
	u32 i;
	CanRxMsg RxMessage;
	if(CAN_MessagePending(CAN1,CAN_FIFO0)==0) return 0;          //没有接收到数据 返回0
	CAN_Receive(CAN1,0,&RxMessage);    //存储接收到得信息
	for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}




