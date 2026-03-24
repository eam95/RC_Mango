/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "NRF24_conf.h"
#include "NRF24_reg_addresses.h"
#include "NRF24.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* LED SPI Control */
#define OFF_LED_EXT 0x00
#define BLUE_LED_EXT 0x01
#define YELLOW_LED_EXT 0x02
#define GREEN_LED_EXT 0x04
#define RED_LED_EXT 0x08

/* nRF24L01+ SPI Control */
uint8_t tx_addr[5] = {0x45, 0x55, 0x67, 0x10, 0x21};
uint16_t data = 0;

#define PLD_S 32
uint8_t dataR[PLD_S];
char colorMsg[PLD_S];
char msgToSend[PLD_S];

int red_count = 0;
int green_count = 0;
int blue_count = 0;
int yellow_count = 0;
int off_count = 0;
int all_count = 0;
                          //
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
void nrf24_setup(void);
void shift_register_send(uint8_t data);

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  //  Set up NRF24L01
  shift_register_send(OFF_LED_EXT); // Only LED A ON
  HAL_Delay(500);
  shift_register_send(BLUE_LED_EXT); // Only LED A ON

  nrf24_setup();

  shift_register_send(OFF_LED_EXT); // Only LED A ON
  HAL_Delay(500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      uint8_t dataR[PLD_S];
      char colorMsg[PLD_S];
      nrf24_listen();

      if(nrf24_data_available())
      {
          HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
          nrf24_receive(dataR, sizeof(dataR));
          // Copy received data to colorMsg and ensure null-termination
          strncpy(colorMsg, (char *)dataR, PLD_S);
          colorMsg[PLD_S - 1] = '\0';
          // Remove trailing newline if present
          size_t len = strlen(colorMsg);
          if (len > 0 && colorMsg[len - 1] == '\n')
          {
              colorMsg[len - 1] = '\0';
          }
          // Compare full color name
          if (strcmp(colorMsg, "red") == 0)
          {
              red_count++;
        	  shift_register_send(RED_LED_EXT);
          }
          else if (strcmp(colorMsg, "green") == 0)
          {
              green_count++;
        	  shift_register_send(GREEN_LED_EXT);
          }
          else if (strcmp(colorMsg, "blue") == 0)
          {
              blue_count++;
        	  shift_register_send(BLUE_LED_EXT);
          }
          else if (strcmp(colorMsg, "yellow") == 0)
          {
              yellow_count++;
        	  shift_register_send(YELLOW_LED_EXT);
          }
          else if (strcmp(colorMsg, "off") == 0)
          {
              off_count++;
        	  shift_register_send(OFF_LED_EXT);
          }
          else if (strcmp(colorMsg, "all") == 0)
          {
        	  all_count++;
              shift_register_send(BLUE_LED_EXT | RED_LED_EXT | GREEN_LED_EXT | YELLOW_LED_EXT);
          }
          else if (strcmp(colorMsg, "count") == 0)
		  {
        	  memset(msgToSend, 0, sizeof(msgToSend)); // Fill with nulls
        	  snprintf(msgToSend, sizeof(msgToSend), "CC,%d,%d,%d,%d,%d,%d", red_count, green_count, blue_count, yellow_count, off_count, all_count);
        	  // msgToSend is now 32 bytes, padded with nulls if the formatted string is shorter
        	  nrf24_stop_listen();
//        	  HAL_Delay(100); // Short delay to ensure the nRF24L01+ is ready
        	  nrf24_transmit(msgToSend, sizeof(msgToSend));
//        	  HAL_Delay(100); // Short delay to ensure the nRF24L01+ is ready

		  }
		  else
		  {
			  // Unknown color command, turn off all LEDs
			  shift_register_send(OFF_LED_EXT);
		  }

          HAL_Delay(1000);
      }
      else
      {
          HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
          HAL_Delay(1000);
      }

      HAL_Delay(100);

  }

}/* End of  main() */

void number_to_string(uint16_t number, char* str, size_t str_size)
{
	snprintf(str, str_size, "%u", number);
}

void shift_register_send(uint8_t data)
{
	HAL_GPIO_WritePin(CS_LED_GPIO_Port, CS_LED_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(CS_LED_GPIO_Port, CS_LED_Pin, GPIO_PIN_SET);
}

void nrf24_setup(void)
{
	// Add your NRF24L01 setup code here if needed
	  ce_high();

	  HAL_Delay(5);

	  ce_low();

	  nrf24_init();

	  nrf24_stop_listen();

	  nrf24_auto_ack_all(auto_ack);
	  nrf24_en_ack_pld(disable);
	  nrf24_dpl(disable);

	  nrf24_set_crc(no_crc, _1byte);

	  nrf24_tx_pwr(_0dbm);
	  nrf24_data_rate(_1mbps);
	  nrf24_set_channel(90);
	  nrf24_set_addr_width(5);

	  nrf24_set_rx_dpl(0, disable);
	  nrf24_set_rx_dpl(1, disable);
	  nrf24_set_rx_dpl(2, disable);
	  nrf24_set_rx_dpl(3, disable);
	  nrf24_set_rx_dpl(4, disable);
	  nrf24_set_rx_dpl(5, disable);

	  nrf24_pipe_pld_size(0, PLD_S);

	  nrf24_auto_retr_delay(4);
	  nrf24_auto_retr_limit(10);

	  nrf24_open_tx_pipe(tx_addr);
	  nrf24_open_rx_pipe(0, tx_addr);
	  ce_high();

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CS_LED_Pin|ce_Pin|csn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IQR_GPIO_Port, IQR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CS_LED_Pin IQR_Pin ce_Pin csn_Pin */
  GPIO_InitStruct.Pin = CS_LED_Pin|IQR_Pin|ce_Pin|csn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : VCP_TX_Pin */
  GPIO_InitStruct.Pin = VCP_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(VCP_TX_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : VCP_RX_Pin */
  GPIO_InitStruct.Pin = VCP_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_USART2;
  HAL_GPIO_Init(VCP_RX_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
