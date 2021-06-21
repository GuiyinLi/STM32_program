#ifndef _dma_h
#define _dma_h
#include "sys.h"
#include "stm32f10x_dma.h"

void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);  //Õ®µ¿≈‰÷√∫Ø ˝

#endif 


