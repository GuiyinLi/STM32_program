#ifndef _dma_h
#define _dma_h
#include "sys.h"

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx);    //开启一次DMA传输

#endif 


