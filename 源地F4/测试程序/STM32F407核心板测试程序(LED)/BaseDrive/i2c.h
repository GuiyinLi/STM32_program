#ifndef __I2C_H
#define __I2C_H			 
#include "stm32f4xx.h"
/* Private define ------------------------------------------------------------*/
#define SCL_H        GPIO_SetBits(GPIOB , GPIO_Pin_6) /*  GPIOB->BSRR = GPIO_Pin_6	   */
#define SCL_L        GPIO_ResetBits(GPIOB , GPIO_Pin_6)/*GPIOB->BRR  = GPIO_Pin_6  */
   
#define SDA_H        GPIO_SetBits(GPIOB , GPIO_Pin_7)  /* GPIOB->BSRR = GPIO_Pin_7 */
#define SDA_L        GPIO_ResetBits(GPIOB , GPIO_Pin_7)	 /*  GPIOB->BRR  = GPIO_Pin_7 */

#define SCL_read     GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_6) /* GPIOB->IDR  & GPIO_Pin_6 */
#define SDA_read     GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7) /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7) 	 */

#define I2C_PageSize  8  /* 24C02Ã¿Ò³8×Ö½Ú */
#define SDA_IN()    {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<(7*2);}
#define SDA_OUT()   {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<(7*2);}
#define ADDR_24LC02		0xA0

/* Private function prototypes -----------------------------------------------*/
void I2C_Configuration(void);
void I2C_Start(void);
uint8_t I2C_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);
uint8_t I2C_ReadOneByte(uint16_t ReadAddress);
void I2C_WriteLenByte(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t length);
void I2C_ReadLenByte(uint16_t ReadAddr, uint8_t *pBuffer,   uint16_t NumToRead);
uint8_t I2C_Test(void);
#endif

/*********************************************************************************************************
** End of File
*********************************************************************************************************/

