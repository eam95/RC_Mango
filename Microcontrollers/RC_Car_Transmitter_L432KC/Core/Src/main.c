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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "NRF24_conf.h"
#include "NRF24_reg_addresses.h"
#include "NRF24.h"
#include "delay.h" // Include delay.h for delay functions
#include "commandReader.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define PLD_S 32
struct UART_Comm
{
	uint8_t RxCmd[PLD_S]; // This will store the received command as a string, which will be parsed to determine the action and parameters to store under the DataToReceive structure.
	uint8_t cmdToTransmit[PLD_S]; // This will store the command to be transmitted as a string, which will be constructed from the DataToTransmit structure and sent through NRF24L01.
	uint8_t UART_Rx_Buffer[PLD_S]; // This buffer will be used for DMA reception of UART data, which will then be copied to RxCmd for processing.
	uint8_t TxData[PLD_S]; // This will store the data to be transmitted as a string, which will be constructed from the DataToTransmit structure and sent through UART.
};

struct RC_CarDataRx
{
   // Disregarding the cmd, all the variables byte size should add up to 18-20bytes
	uint8_t cmd[PLD_S]; // Will grab all the structure variables compress to string and transmit through NRF24L01
	int32_t a_x; // Store the acceleration in x-axis
    int32_t a_y; // Store the acceleration in y-axis
    int32_t a_z; // Store the acceleration in z-axis
    uint16_t distance_cm; // Store the distance measurement in centimeters
    char stationaryFlag;  // This is Flag to indicate back to the transmitter whether there is a stationary object when the car is accelerating.
    uint32_t timestamp; // in milliseconds

};



typedef enum
{
    STATE_WAIT,
    STATE_TRANSMIT,
	STATE_TEST_UART,
	STATE_RECEIVE
} FSM_State;
FSM_State current_state = STATE_WAIT;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */

//////  Commands Declarations  //////////////////////////////////////
struct RC_CarDataRx RC_Data = {0};    // This initializes all members to 0
struct UART_Comm Msg = {0}; // This initializes all members to 0


/* LED SPI Control */
#define OFF_LED_EXT     0x00
#define BLUE_LED_EXT    0x01
#define YELLOW_LED_EXT  0x02
#define GREEN_LED_EXT   0x04
#define RED_LED_EXT     0x08

// Payload Size



/* nRF24L01+ SPI Control */
uint8_t tx_addr[5] = {0x45, 0x55, 0x67, 0x10, 0x21};
uint16_t data = 0;
uint8_t toggleFlag = 0;

// For UART DMA reception
uint8_t UART2_rxBuffer[PLD_S]; // DMA buffer

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
void nrf24_setup(void);
void shift_register_send(uint8_t data);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//  Set up NRF24L01

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_DMA(&huart2, Msg.UART_Rx_Buffer, PLD_S); // Start UART reception in DMA mode, storing received data in the buffer for processing

  // Start Timer 6 in interrupt mode for LED toggling
  HAL_TIM_Base_Start_IT(&htim6);

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
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  switch (current_state)
	  {
	  case STATE_WAIT:
		  // Just wait firm-ever :)
		  // Just had to be very cheesey :p
		  break;

	  case STATE_TEST_UART:
		  // Here I will print a message to test UART functionality
		  char* test_msg = "UART DMA RX Test Successful!\r\n";
		  HAL_UART_Transmit(&huart2, (uint8_t*)test_msg, strlen(test_msg), HAL_MAX_DELAY);
		  HAL_Delay(1000);
//		  current_state = STATE_WAIT; // Return to WAIT state
		  break;

	  case STATE_RECEIVE:
		  // After transmitting data, the nRF24L01+ will switch to receive mode to wait for the RC car data,
		  // I'm gonna test the polling method.
		  if (nrf24_data_available())
		  {
			  shift_register_send(BLUE_LED_EXT || RED_LED_EXT); // Turn on LED when receiving
			  char RxData[PLD_S] = {0};
			  nrf24_receive(RC_Data.cmd, sizeof(RC_Data.cmd));
			  nrf24_clear_rx_dr(); // Clear the RX_DR flag after receiving data
			  memcpy(RxData, RC_Data.cmd, PLD_S);
			  uint8_t comma_index = 0;
			  char timeStamp_str[16] = {0};
			  char distance_str[16] = {0};
			  char a_x_str[16] = {0};
			  char a_y_str[16] = {0};
			  char a_z_str[16] = {0};
			  parse_uart_value(RxData, timeStamp_str,&comma_index);
			  parse_uart_value(RxData, distance_str,&comma_index);
			  parse_uart_value(RxData, a_x_str,&comma_index);
			  parse_uart_value(RxData, a_y_str,&comma_index);
			  parse_uart_value(RxData, a_z_str,&comma_index);
			  // The data received will be converted to the corresponding data type and stored in the Receive structure for further processing or decision making.
			  // For troubleshooting purpose.
			  RC_Data.timestamp = (uint32_t)atoi(timeStamp_str);
			  RC_Data.distance_cm = (uint16_t)atoi(distance_str);
			  RC_Data.a_x = (int32_t)atoi(a_x_str);
			  RC_Data.a_y = (int32_t)atoi(a_y_str);
			  RC_Data.a_z = (int32_t)atoi(a_z_str);
			  // Transmit the data to UART TX, so gui can read the data through serial communication and display it on the GUI.
//			  uint16_t len = snprintf((char *)Msg.TxData, sizeof(Msg.TxData), "Timestamp: %lu ms, Distance: %u cm, Acceleration: (%ld, %ld, %ld) mg\r\n", Receive.timestamp, Receive.distance_cm, Receive.a_x, Receive.a_y, Receive.a_z);
			  // Send the Receive.cmd
			  memcpy(Msg.TxData, RC_Data.cmd, PLD_S);
//			  HAL_UART_Transmit(&huart2, Msg.TxData, strlen((char *)Msg.TxData), HAL_MAX_DELAY);
			  current_state = STATE_WAIT; // Go back to wait state after receiving data
		  }
		  // Stay in receive mode while waiting for data

		  break;

	  case STATE_TRANSMIT:
		  shift_register_send(YELLOW_LED_EXT || RED_LED_EXT); // Only LED A ON
		  nrf24_transmit(Msg.RxCmd, sizeof(Msg.RxCmd));
		  current_state = STATE_WAIT;
		  break;
	  }
  }
  /* USER CODE END 3 */
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
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 80-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 10000-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */
  /* Enable TIM6 global interrupt */
  HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

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
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CS_LED_Pin ce_Pin csn_Pin */
  GPIO_InitStruct.Pin = CS_LED_Pin|ce_Pin|csn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : IQR_Pin */
  GPIO_InitStruct.Pin = IQR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IQR_GPIO_Port, &GPIO_InitStruct);

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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		// Timer toggles every 10ms
		// Use this for timeout mechanism when waiting for receive data
		if (toggleFlag == 0)
		{
			// To switch to transmitt state
			toggleFlag = 1;
			shift_register_send(YELLOW_LED_EXT); // Turn on LED when waiting for data, indicating timeout if it stays on for too long
//			nrf24_stop_listen();
			// After setting listen mode, it will go to wait until there is UART interrupt to switch to transmit state, or there is data received through NRF24L01 to switch to receive state.
//			current_state = STATE_WAIT;

		}
		else if (toggleFlag == 1)
		{
			// to switch to receive state
			toggleFlag = 0;
			shift_register_send(BLUE_LED_EXT); // Turn off LED after timeout, indicating ready to receive data again
//			nrf24_listen(); // Switch back to listen mode to wait for data again
//			current_state = STATE_RECEIVE; // Switch to receive state to wait for data after timeout
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == IQR_Pin)
    {
		// IRQ interrupt handling for the NRF24L01
    	// When in receive mode and data arrives, the IRQ pin will trigger this interrupt
    	// This allows immediate data reception without waiting for polling
    	if (current_state == STATE_RECEIVE)
    	{
    		// The IRQ indicates data is ready in RX FIFO
    		// The main loop will handle the actual data reception
    		// We just ensure we stay in RECEIVE state to process it
    		toggleFlag = 0; // Reset timeout counter since we got an IRQ
    	}
    }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
    	shift_register_send(GREEN_LED_EXT);
    	// Copy the received data to your message buffer
        memcpy(Msg.RxCmd, Msg.UART_Rx_Buffer, PLD_S);
        // Restart DMA reception for next message
        HAL_UART_Receive_DMA(&huart2, Msg.UART_Rx_Buffer, PLD_S);
		shift_register_send(OFF_LED_EXT); // Only LED A ON
		current_state = STATE_TRANSMIT;
    }
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
