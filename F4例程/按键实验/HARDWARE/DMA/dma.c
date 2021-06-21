#include "dma.h"

u16 DMA2_MEM_LEN;//保存DMA每次数据传送的长度 	    
//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量  
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	if((u32)DMA_Streamx>(u32)DMA2)    //得到当前的数据流是属于DMA1还是DMA2
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);      //使能DMA1时钟
	else
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);      //使能DMA1时钟
	
	DMA2_MEM_LEN=cndtr;                                   	//把设置的值付给Cndtr
	
	DMA_DeInit(DMA_Streamx);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//等待DMA可配置 
	
	DMA_InitStructure.DMA_Channel=DMA_CHx;               //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr=cpar;           //DMA外设基地址
	DMA_InitStructure.DMA_Memory0BaseAddr=cmar;              //DMA内存基地址
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory;     //数据传输方向 从外设到内存
	DMA_InitStructure.DMA_BufferSize=cndtr;                   //DMA通道的DMA缓存大小
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;    //外设基地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;   //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;    //外设数据宽度为32位
	DMA_InitStructure.DMA_MemoryDataSize=DMA_PeripheralDataSize_HalfWord;   //存储器数据宽度为32位   必须为32位，这里会影响到ADC的最大值！！
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;             //工作在循环缓存模式 适用于多路ADC
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;     //设置请求优先级 中
	DMA_InitStructure.DMA_FIFOMode=DISABLE;               //关闭FIFO模式
	DMA_InitStructure.DMA_FIFOThreshold=DMA_FIFOThreshold_Full;//FIFO阈值级别为满
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA_Streamx,&DMA_InitStructure);               //初始化
}

//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx)
{
	DMA_Cmd(DMA_Streamx,DISABLE);                            //关闭
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//确保DMA可以被设置
		
	DMA_SetCurrDataCounter(DMA_Streamx,DMA2_MEM_LEN);    //DMA通道DMA缓存大小
	
	DMA_Cmd(DMA_Streamx,ENABLE);                           //使能
}




