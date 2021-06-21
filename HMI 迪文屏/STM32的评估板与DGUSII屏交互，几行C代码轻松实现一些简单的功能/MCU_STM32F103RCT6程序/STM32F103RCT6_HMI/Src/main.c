
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);



void PLC_Data_Wr2HMI(uint16_t HMI_start_addr,uint16_t PLC_return_data_start_addr,uint8_t len);
void WRITE_HMI(uint16_t addr,uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4,uint8_t len);
void READ_HMI(uint16_t addr,uint8_t num);
void CLOSE_OPEN_IO(uint8_t hmi_key_value);
void SCAN_KEY(uint8_t io_key_value);  
void delay_ms(uint16_t nms);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
uint8_t HMI_SEND_BUF[20] = {0};
uint8_t usart1_counter = 0;
uint8_t usart1_data;
uint8_t usart1_rxBuf[50];
uint8_t usart1_txBuf[50];
uint8_t HMI_HANDLEFLAG;

uint8_t LED_FLAG[5] = {0};
uint8_t HMI_FLAG[6] = {0};
uint8_t TEMP = 25;//上电温度
uint8_t VOLUME = 50;//上电音量
uint16_t PLAY = 0;//
uint8_t PLAY_N = 0;
uint8_t UP_POWER_FLAG=0;

int main(void)
{
	uint8_t i;
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
				
  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {		
		HAL_UART_Receive_IT(&huart1,&usart1_data,1);
		if(UP_POWER_FLAG == 0)
		{
			UP_POWER_FLAG = 1;
			HMI_SEND_BUF[0] = TEMP;
			PLC_Data_Wr2HMI(0X1110,0,1);   //用于显示  1：现在就发HMI_SEND_BUF[0] 只有一个数据 len是数据个数
			delay_ms(100);
			HMI_SEND_BUF[0] = VOLUME;
			PLC_Data_Wr2HMI(0X1210,0,1);//音量显示
			delay_ms(100);
			WRITE_HMI(0X00A1,VOLUME,0x00,0,0,2);//默认音量50
			delay_ms(100);
			for(i=0;i<5;i++)
			{
				HMI_SEND_BUF[i] = 0;
			}
			for(i=0;i<20;i++)
			{
				usart1_rxBuf[i] = 0x00;
			}
			HMI_HANDLEFLAG = 0;
		}
		if(PLAY ==1)
		{
			READ_HMI(0x00a1,1);
			delay_ms(100);
			if(HMI_HANDLEFLAG)
			{
				if(usart1_rxBuf[8] == 0)
				{
					PLAY = 0;
					for(i=0;i<50;i++)
					{
						usart1_rxBuf[i] = 0x00;
					}
					usart1_counter = 0;
				}
			}
		}
		SCAN_KEY(0);
		if(HMI_HANDLEFLAG)
		{
			HMI_HANDLEFLAG = 0;
			CLOSE_OPEN_IO(0x01);
			CLOSE_OPEN_IO(0x02);
			CLOSE_OPEN_IO(0x03);
			CLOSE_OPEN_IO(0x04);
			CLOSE_OPEN_IO(0x05);
			for(i=0;i<50;i++)
			{
				usart1_rxBuf[i] = 0x00;
			}
			usart1_counter = 0;
		}			
			
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

void PLC_Data_Wr2HMI(uint16_t HMI_start_addr,uint16_t PLC_return_data_start_addr,uint8_t len)
{
	uint8_t i;
	usart1_txBuf[0]=0x5a;
	usart1_txBuf[1]=0xa5;
	usart1_txBuf[2]=len*2+3;
	usart1_txBuf[3]=0x82;
	usart1_txBuf[4]=((uint8_t)(HMI_start_addr>>8))&0XFF;
	usart1_txBuf[5]=(uint8_t)(HMI_start_addr&0XFF);     //格式数据
	for(i=0;i<len;i++)                               //有效数据
	{
		usart1_txBuf[i*2+6]=((HMI_SEND_BUF[i+PLC_return_data_start_addr] >>8)&0xff);
		usart1_txBuf[i*2+7]=(HMI_SEND_BUF[i+PLC_return_data_start_addr] &0xff);
	}
	HAL_UART_Transmit(&huart1, usart1_txBuf, len*2+6, 10);
}

void WRITE_HMI(uint16_t addr,uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4,uint8_t len)
{
		usart1_txBuf[0]=0x5a;
		usart1_txBuf[1]=0xa5;
		usart1_txBuf[2]=len*2+3;
		usart1_txBuf[3]=0x82;
		usart1_txBuf[4]=(uint8_t)((addr>>8)&0xff);
		usart1_txBuf[5]=(uint8_t)(addr&0xff);
		usart1_txBuf[6]=data1;
		usart1_txBuf[7]=data2;
		usart1_txBuf[8]=data3;
		usart1_txBuf[9]=data4;
		HAL_UART_Transmit(&huart1, usart1_txBuf, len*2+6 , 20);
}

void READ_HMI(uint16_t addr,uint8_t num)
{
		usart1_txBuf[0]=0x5a;
		usart1_txBuf[1]=0xa5;
		usart1_txBuf[2]=0x04;
		usart1_txBuf[3]=0x83;
		usart1_txBuf[4]=(uint8_t)((addr>>8)&0xff);
		usart1_txBuf[5]=(uint8_t)(addr&0xff);
		usart1_txBuf[6]=num;

		HAL_UART_Transmit(&huart1, usart1_txBuf, 7 , 20);
}

void CLOSE_OPEN_IO(uint8_t hmi_key_value)
{ 
	uint8_t i;
	if(usart1_rxBuf[7] == 0x00&&usart1_rxBuf[8] == hmi_key_value)//LED1
	{
		if(LED_FLAG[hmi_key_value-1] == 0)
		{
		  if(hmi_key_value == 0x01)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
			}
			if(hmi_key_value == 0x02)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
			}
			if(hmi_key_value == 0x03)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
			}
			if(hmi_key_value == 0x04)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
			}
			if(hmi_key_value == 0x05)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
			}
			LED_FLAG[hmi_key_value-1] = 1;
		}
		else if(LED_FLAG[hmi_key_value-1] == 1)
		{
		  if(hmi_key_value == 0x01)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
			}
			if(hmi_key_value == 0x02)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
			}
			if(hmi_key_value == 0x03)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
			}
			if(hmi_key_value == 0x04)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
			}
			if(hmi_key_value == 0x05)
			{	
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
			}
			LED_FLAG[hmi_key_value-1] = 0;
		}
		for(i=0;i<9;i++)
		{
			usart1_rxBuf[i] = 0xff;
		}
		for(i=0;i<5;i++)
		{
			HMI_SEND_BUF[i] = LED_FLAG[i];
		}
		PLC_Data_Wr2HMI(0X1000,0,5);
		delay_ms(50);
	}
}

void SCAN_KEY(uint8_t io_key_value)  
{ 
	uint8_t i;
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==GPIO_PIN_RESET)//KEY1,屏幕开关
		{
			delay_ms(150);//去抖动
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==GPIO_PIN_RESET)//KEY1,屏幕开关
			{
				if(HMI_FLAG[0] == 0)
				{
					WRITE_HMI(0X82,0x00,0x00,0,0,1);//关屏幕
					HMI_FLAG[0] = 1;
				}else if(HMI_FLAG[0] == 1)
				{
					WRITE_HMI(0X82,0X64,0x00,0,0,1);//开屏幕
					HMI_FLAG[0] = 0;
				}
			}
			for(i=0;i<50;i++)
			{
				usart1_rxBuf[i] = 0x00;
			}
			usart1_counter = 0;
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==GPIO_PIN_RESET)//KEY2，温度+
		{
			delay_ms(150);//去抖动
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==GPIO_PIN_RESET)
			{
				TEMP += 1;
				HMI_SEND_BUF[0] = TEMP;
				PLC_Data_Wr2HMI(0X1110,0,1);
			}
			for(i=0;i<50;i++)
			{
				usart1_rxBuf[i] = 0x00;
			}
			usart1_counter = 0;
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)==GPIO_PIN_RESET)//KEY3，温度-
		{
			delay_ms(150);//去抖动
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)==GPIO_PIN_RESET)
			{
				TEMP -= 1;
				HMI_SEND_BUF[0] = TEMP;
				PLC_Data_Wr2HMI(0X1110,0,1);
			}
			for(i=0;i<50;i++)
			{
				usart1_rxBuf[i] = 0x00;
			}
			usart1_counter = 0;
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)==GPIO_PIN_RESET)//KEY4,播放停止/开始
		{
			delay_ms(150);//去抖动
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)==GPIO_PIN_RESET)
			{
				if(PLAY == 0)
				{			
					WRITE_HMI(0X00A0,0X00,0x0E,0,0,1);
					PLAY =1;//播放状态
				}else if(PLAY == 1)
				{						
					WRITE_HMI(0X00A0,0X00,0x00,0,0,1);
					PLAY =0;//暂停状态					
				}
			}
			for(i=0;i<50;i++)
			{
				usart1_rxBuf[i] = 0x00;
			}
			usart1_counter = 0;
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==GPIO_PIN_RESET)//KEY5，音量+
		{
			delay_ms(150);//去抖动
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==GPIO_PIN_RESET)
			{
				if(VOLUME<250)
				{
					VOLUME += 10;
					WRITE_HMI(0X00A1,VOLUME,0x00,0,0,2);//音量加10
					delay_ms(50);
					HMI_SEND_BUF[0] = VOLUME;
					PLC_Data_Wr2HMI(0X1210,0,1);//音量显示
					delay_ms(50);
				}
			}
			for(i=0;i<50;i++)
			{
				usart1_rxBuf[i] = 0x00;
			}
			usart1_counter = 0;
		}
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==GPIO_PIN_RESET)//KEY6，音量-
		{
			delay_ms(150);//去抖动
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==GPIO_PIN_RESET)
			{
				if(VOLUME >= 10)
				{
					VOLUME -= 10;
					WRITE_HMI(0X00A1,VOLUME,0x00,0,0,2);//音量减10
					delay_ms(50);
					HMI_SEND_BUF[0] = VOLUME;
					PLC_Data_Wr2HMI(0X1210,0,1);//音量显示
					delay_ms(50);
				}			
			}
			for(i=0;i<50;i++)
			{
				usart1_rxBuf[i] = 0x00;
			}
			usart1_counter = 0;
		}

}

void delay_ms(uint16_t nms)
{
	uint16_t i,j;
	for(i=0;i<nms;i++)
	{
		for(j=0;j<10000;j++)
		{
			;
		}
	}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_10, GPIO_PIN_SET);

  /*Configure GPIO pins : PC13 PC0 PC2 PC3 
                           PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 PB12 PB13 
                           PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
