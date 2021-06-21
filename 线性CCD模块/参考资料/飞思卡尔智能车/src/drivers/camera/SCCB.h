#ifndef __SCCB_H
#define __SCCB_H


#define SCL_H()         PTB10_OUT = 1
#define SCL_L()         PTB10_OUT = 0
#define	SCL_DDR_OUT() 	DDRB10 = 1
#define	SCL_DDR_IN() 	DDRB10 = 0

#define SDA_H()         PTB11_OUT = 1
#define SDA_L()         PTB11_OUT = 0
#define SDA_IN()      	PTB11_IN
#define SDA_DDR_OUT()	DDRB11 = 1
#define SDA_DDR_IN()	DDRB11 = 0

#define ADDR_OV7725   0x42

#define SCCB_DELAY()	SCCB_delay(100)	


void SCCB_GPIO_init(void);
int SCCB_WriteByte( u16 WriteAddress , u8 SendByte);
int SCCB_ReadByte(u8* pBuffer,   u16 length,   u8 ReadAddress);

static void SCCB_delay(u16 i);
#endif 
