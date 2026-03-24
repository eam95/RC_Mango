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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "GarminLidarLiteV3.h"
#include "stm32h7xx_hal.h"
#include "delay.h" // Include delay.h for delay functions
#include "commandReader.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_DMA_RX_SIZE 32

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim24;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
//typedef enum {
//	WAIT_STATE,
//	SIMPLE_MEASUREMENT,
//	CHANGE_SENSITIVITY,
//	STRING_STATE,
//	ACQ_CHANGE
//} StateType;
//StateType currentState = WAIT_STATE;

typedef enum {
	WAIT_STATE,
	SIMPLE_MEASUREMENT,
	CHANGE_SENSITIVITY,
	ACQ_CHANGE,
	ACQ_COUNT_TEST,
	ACQ_COUNT_INIT,
	TIMER_TEST_DELAY,
	BURST_INIT,
	BURST_TEST,
	SIMPLE_MEASUREMENT_SEMIBURST
} StateType;
StateType currentState = SIMPLE_MEASUREMENT;
//StateType currentState = ACQ_COUNT_INIT;

// For UART message processing
char uart_current_msg[UART_DMA_RX_SIZE];

// UART DMA Buffer
uint8_t UART3_rxBuffer[UART_DMA_RX_SIZE]; // DMA buffer

// Time tracking
volatile uint32_t seconds_counter = 0;

// Sensitivity level
volatile uint8_t sensitivity_level = 100;

// Sampling time set
volatile uint32_t sample_interval_ms = 1000;

// Acquisition count default
volatile uint8_t acq_count_value = 10;

// Sampling time set
volatile uint32_t N_samples = 20;

// semi burst count for SIMPLE_MEASUREMENT_SEMIBURST
volatile uint16_t burst_count_value = 5;

// Acq_command change parameters
volatile uint8_t acq_cmd_param = TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION;

// Rate of ticks is 1MHz with prescalar of 100 (assuming 100MHz clock)
// 1ticks = 1us
uint32_t time_period_set = 10000; // Initial timer period in ticks to represent 10ms
volatile uint8_t measure_flag = 0; // Flag set in timer interrupt to trigger measurement


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM24_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_USART3_UART_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_TIM24_Init();
  /* USER CODE BEGIN 2 */

  GarLiteV3_Init();
  HAL_UART_Receive_DMA(&huart3, UART3_rxBuffer, UART_DMA_RX_SIZE);
  // Start TIM2 with interrupt
  HAL_TIM_Base_Start_IT(&htim2);

  uint16_t distanceCm = 0;
  uint32_t timeSet = adjusteTimeForTicks(200); // Adjusted for timer ticks
  char msg[32];
  uint32_t start_time = 0; // will be added as counting in us for each periodic interrupt
  uint32_t i = 0;
  char endmsg[32] = "END\r\n";
  uint16_t NumOfBursts = 5; // Set number of bursts for burst measurement mode
  uint8_t burst_delay =10;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  switch (currentState)
	  {
	  	  case WAIT_STATE:
	  		  // Do nothing, wait for command
	  		  break;

	  	  case TIMER_TEST_DELAY:
	  		  // Test timer delay function
  			  HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
  			  delay_us(timeSet); // Delay in us
	  		  break;

	  	  case CHANGE_SENSITIVITY:
	  		  // Change sensitivity level (example value here)
	  		  config_THRESHOLD_BYPASS_reg(TH_SENSITIVITY_LEVEL(sensitivity_level),1);
	  		  currentState = WAIT_STATE; // Return to wait state
	  		  break;

	  	  case ACQ_CHANGE:
	  		  // Change acquisition parameters (not implemented in this example)
	  		  config_ACQ_COMMAND_reg(acq_cmd_param,1);
	  		  currentState = WAIT_STATE; // Return to wait state
	  		  break;

	  	  case SIMPLE_MEASUREMENT:
	  		  // Perform simple measurement with time stamp
	  		  // Reset time tracking
	  		  start_time = 0; // will be added as counting in us for each periodic interrupt
	  		  memset(msg, 0, sizeof(msg)); // Clears all elements of the array
	  		  i = 0;
	  		  // Configure timer period based on sample_interval_ms
	  		  time_period_set = sample_interval_ms * 1000; // Convert ms to us
	  		  // Reinitialize timer with new period
	  		  MX_TIM2_Init();
	  		  // Set up for loop up to N samples and exit SIMPLE_MEASUREMENT state after N samples
	  		  while (i < N_samples)
	  		  {
				  // Perform measurement once periodic interrupt is detected
				  // Wait for the timer period to elapse
	  			  if (measure_flag == 1)
	  			  {
	  				  measure_flag = 0; // Reset flag

	  				  // Take distance measurement
	  				  simple_measurement(&distanceCm,0);

	  				  // Format: "time_ms,distance_cm\r\n"
	  				  int len = snprintf(msg, sizeof(msg), "%lu,%u\r\n",start_time, distanceCm);
	  				  if (len > 0)
	  				  {
	  					  HAL_UART_Transmit(&huart3, (uint8_t *)msg, (uint16_t)len, HAL_MAX_DELAY);
	  				  }
	  				  i++; // Increment sample count
	  				  start_time += sample_interval_ms; // Increment time stamp in us
	  				  memset(msg, 0, sizeof(msg)); // Clears all elements of the array

	  			  }

	  		  }
	  		  // Send 'END' message to indicate end of measurement series
	  		  HAL_UART_Transmit(&huart3, (uint8_t *)endmsg, (uint16_t)strlen((char *)endmsg), HAL_MAX_DELAY);
	  		  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET); // Turn off green LED
	  		  currentState = WAIT_STATE; // Return to wait state
	  		  break;

	  	  case SIMPLE_MEASUREMENT_SEMIBURST:
	  		  // Perform simple measurement with semi-burst mode
	  		  uint16_t burst_gathered[10]; // Array to hold burst measurements
	  		  // Reset time tracking
	  		  start_time = 0; // will be added as counting in us for each periodic interrupt
	  		  memset(msg, 0, sizeof(msg)); // Clears all elements of the array
	  		  i = 0;
	  		  // Configure timer period based on sample_interval_ms
	  		  time_period_set = sample_interval_ms * 1000; // Convert ms to us
	  		  // Reinitialize timer with new period
	  		  MX_TIM2_Init();
	  		  // Set up for loop up to N samples and exit SIMPLE_MEASUREMENT state after N samples
	  		  while (i < N_samples)
	  		  {
				  // Perform measurement once periodic interrupt is detected
				  // Wait for the timer period to elapse
	  			  if (measure_flag == 1)
	  			  {
	  				  measure_flag = 0; // Reset flag
	  				  // Take semi-burst distance measurements
	  				  for (uint16_t j=0; j<burst_count_value; j++)
	  				  {
	  					  simple_measurement(&burst_gathered[j],0);
	  				  }
	  				  // Average burst measurements
	  				  uint32_t burst_sum =0;
	  				  for (uint16_t j=0; j<burst_count_value; j++)
	  				  {
	  					  burst_sum += burst_gathered[j];
	  				  }
	  				  distanceCm = burst_sum / burst_count_value;
	  				  // Format: "time_ms,distance_cm\r\n"
	  				  int len = snprintf(msg, sizeof(msg), "%lu,%u\r\n",start_time, distanceCm);
	  				  if (len > 0)
	  				  {
	  					  HAL_UART_Transmit(&huart3, (uint8_t *)msg, (uint16_t)len, HAL_MAX_DELAY);
	  				  }
	  				  i++; // Increment sample count
	  				  start_time += sample_interval_ms; // Increment time stamp in us
	  				  memset(msg, 0, sizeof(msg)); // Clears all elements of the array

	  			  }

	  		  }
	  		  // Send 'END' message to indicate end of measurement series
	  		  HAL_UART_Transmit(&huart3, (uint8_t *)endmsg, (uint16_t)strlen((char *)endmsg), HAL_MAX_DELAY);
	  		  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET); // Turn off green LED

	  		  currentState = WAIT_STATE; // Return to wait state
	  		  break;

	  	  case ACQ_COUNT_INIT:
	  		  // Change acquisition parameters to count test (not implemented in this example)
	  		  config_SIG_COUNT_VAL_reg(acq_count_value,1); // Set to 1 for count test
	  		  // Start a measurement *immediately*
	  		  config_ACQ_COMMAND_reg(acq_cmd_param, 0);
	  		  timeSet = adjusteTimeForTicks(200); // Adjusted for timer ticks
	  		  currentState = ACQ_COUNT_TEST;
	  		  break;


	  	  case ACQ_COUNT_TEST:
	  		  // Test after initialization
	  		  // Perform acquisition measurement with time stamp
	  		  // Reset time tracking
	  		  start_time = 0; // will be added as counting in us for each periodic interrupt
	  		  i = 0;
	  		  memset(msg, 0, sizeof(msg)); // Clears all elements of the array
	  		  // Configure timer period based on sample_interval_ms
	  		  time_period_set = sample_interval_ms * 1000; // Convert ms to us
	  		  // Reinitialize timer with new period
	  		  MX_TIM2_Init();
	  		  // Set up for loop up to N samples and exit SIMPLE_MEASUREMENT state after N samples
	  		  while (i < N_samples)
	  		  {
				  // Perform measurement once periodic interrupt is detected
				  // Wait for the timer period to elapse
	  			  if (measure_flag == 1)
	  			  {
	  				  measure_flag = 0; // Reset flag

	  				  // Take distance measurement
	  				  acq_count_measurements(&distanceCm,0);
	  				  // Format: "time_ms,distance_cm\r\n"
	  				  int len = snprintf(msg, sizeof(msg), "%lu,%u\r\n",start_time, distanceCm);
	  				  if (len > 0)
	  				  {
	  					  HAL_UART_Transmit(&huart3, (uint8_t *)msg, (uint16_t)len, HAL_MAX_DELAY);
	  				  }
	  				  i++; // Increment sample count
	  				  start_time += sample_interval_ms; // Increment time stamp in us
	  				  memset(msg, 0, sizeof(msg)); // Clears all elements of the array

	  			  }

	  		  }
	  		  // Send 'END' message to indicate end of measurement series
	  		  HAL_UART_Transmit(&huart3, (uint8_t *)endmsg, (uint16_t)strlen((char *)endmsg), HAL_MAX_DELAY);
	  		  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET); // Turn off green LED
	  		  currentState = WAIT_STATE; // Return to wait state
	  		  break;

	  	case BURST_INIT:
	  	    // Step 1: Configure ACQ_CONFIG_REG first
	  	    config_ACQ_CONFIG_REG(
	  	        ENABLE_REFERENCE_PROCESS,      // Keep reference process enabled
	  	        ENABLE_MEASURE_DELAY,          // Enable custom delay
	  	        DISABLE_REFERENCE_FILTER,
				ENABLE_QUICK_TERMINATION,      // Enable for faster measurements
	  	        DISABLE_REFERENCE_ACQ_COUNT,
	  	        DEFAULT_PWM_Mode,
	  	        1
	  	    );

	  	    // Step 2: Set MEASURE_DELAY BEFORE triggering
	  	    config_MEASURE_DELAY_reg(burst_delay, 1);  // Set delay to 10 (example value)

	  	    // Step 3: Set burst count
	  	    config_OUTER_LOOP_COUNT_reg(NumOfBursts, 1);

	  	    // Step 4: NOW trigger the burst (do this LAST!)
	  	    config_ACQ_COMMAND_reg(TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION, 1);

	  	    // Step 5: Move to collection state
	  	    currentState = BURST_TEST;
	  	    break;

	  	case BURST_TEST:
//	  		The burst measurement works but based on waveforms the timing is not consistent.
//	  		In our case the sampling by single/ acquisition count modes is more reliable by sampling time.
	  	    char msg[64];
	  	    uint16_t data[NumOfBursts];
	  	    // Read each measurement as it becomes available
	  	    HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
	  	    for (uint16_t i = 0; i < NumOfBursts; i++)
	  	    {
	  	        // Wait for measurement ready
	  	    	HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	  	        while ((read_STATUS_reg(0) & BUSY_FLAG) != 0); // Wait until BUSY = 0, then will measure
	  	    	HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	  	        // Read distance

	  	    	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	  	        uint8_t high = read_FULL_DELAY_HIGH_reg(0);
	  	        uint8_t low = read_FULL_DELAY_LOW_reg(0);
	  	        uint16_t distance = ((uint16_t)high << 8) | low;
	  	        data[i] = distance;
	  	        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	  	    }

	  	    HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
	  	    // Average all measurements
	  	    uint32_t dataSum = 0;
	  	    for (uint16_t i = 0; i < NumOfBursts; i++)
	  	    {
	  	    	dataSum += data[i];
	  	    }
	  	    uint16_t dataAvg = dataSum / NumOfBursts;
	  	    // Send averaged result over UART
	  	    int len = snprintf(msg, sizeof(msg), "BURST_AVG_DISTANCE_CM: %u\r\n", dataAvg);
	  	    if (len > 0)
	  	    {
	  	        HAL_UART_Transmit(&huart3, (uint8_t *)msg, (uint16_t)len, HAL_MAX_DELAY);
	  	    }
	  	    // ✅ Trigger next burst!
	  	    config_ACQ_COMMAND_reg(TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION, 0);
//	  	    HAL_Delay(10);
//	  	    currentState = BURST_INIT;

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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 31;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 2048;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00C042E4;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 250-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = time_period_set-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM24 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM24_Init(void)
{

  /* USER CODE BEGIN TIM24_Init 0 */

  /* USER CODE END TIM24_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM24_Init 1 */

  /* USER CODE END TIM24_Init 1 */
  htim24.Instance = TIM24;
  htim24.Init.Prescaler = 25-1;
  htim24.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim24.Init.Period = 4294967295;
  htim24.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim24.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim24) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim24, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim24, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM24_Init 2 */

  /* USER CODE END TIM24_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_GREEN_Pin LED_RED_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin|LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_YELLOW_Pin */
  GPIO_InitStruct.Pin = LED_YELLOW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_YELLOW_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Timer interrupt callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    { // Check if the interrupt is from TIM2
    	if (currentState == SIMPLE_MEASUREMENT || currentState == ACQ_COUNT_TEST || currentState == SIMPLE_MEASUREMENT_SEMIBURST)
		{
        	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        	measure_flag = 1; // Set flag to indicate measurement time
		}


    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        // Copy DMA buffer so we can safely modify/terminate it
        memcpy(uart_current_msg, UART3_rxBuffer, UART_DMA_RX_SIZE);

        // Parse the command, extracting command string
		char cmd_str[UART_DMA_RX_SIZE] = {0};
		uint8_t comma_index = 0;
		parse_uart_command(uart_current_msg, cmd_str, &comma_index);


        // Compare and act on commands
        if (strcmp(cmd_str, "oneMeas") == 0)
        {
        	uint8_t index = comma_index; // Value starts after comma
        	char sample_interval_str[8] = {0};
        	parse_uart_value(uart_current_msg, sample_interval_str, &index);
        	sample_interval_ms = (uint32_t)atoi(sample_interval_str);

        	char N_samples_str[8] = {0};
        	parse_uart_value(uart_current_msg, N_samples_str, &index);
        	N_samples = (uint32_t)atoi(N_samples_str);

        	currentState = SIMPLE_MEASUREMENT;
        }
        else if (strcmp(cmd_str, "acqMeas") == 0)
        {
        	uint8_t index = comma_index; // Value starts after comma
        	char sample_interval_str[8] = {0};
        	parse_uart_value(uart_current_msg, sample_interval_str, &index);
        	sample_interval_ms = (uint32_t)atoi(sample_interval_str);

        	char N_samples_str[8] = {0};
        	parse_uart_value(uart_current_msg, N_samples_str, &index);
        	N_samples = (uint32_t)atoi(N_samples_str);

        	char acq_count_str[8] = {0};
        	parse_uart_value(uart_current_msg, acq_count_str, &index);
        	acq_count_value = (uint8_t)atoi(acq_count_str);

        	currentState = ACQ_COUNT_INIT;
        }

        else if (strcmp(cmd_str, "sBurst") == 0)
        {
        	uint8_t index = comma_index; // Value starts after comma
        	char sample_interval_str[8] = {0};
        	parse_uart_value(uart_current_msg, sample_interval_str, &index);
        	sample_interval_ms = (uint32_t)atoi(sample_interval_str);

        	char N_samples_str[8] = {0};
        	parse_uart_value(uart_current_msg, N_samples_str, &index);
        	N_samples = (uint32_t)atoi(N_samples_str);

        	char burst_count_str[8] = {0};
        	parse_uart_value(uart_current_msg, burst_count_str, &index);
        	burst_count_value = (uint8_t)atoi(burst_count_str);

        	currentState = SIMPLE_MEASUREMENT_SEMIBURST;
        }

        else if (strcmp(cmd_str, "wait") == 0)
        {
            // Send a message indicating wait state
        	snprintf(uart_current_msg, sizeof(uart_current_msg), "Entering wait state\r\n");
        	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET); // Turn off green LED
        	measure_flag = 0; // Reset flag
        	HAL_UART_Transmit(&huart3, (uint8_t *)uart_current_msg, (uint16_t)strlen(uart_current_msg), HAL_MAX_DELAY);
        	currentState = WAIT_STATE;
        }

        else if (strcmp(cmd_str, "sensChange") == 0)
        {
        				// Get sensitivity value
			uint8_t index = comma_index; // Value starts after comma
			char sensitivity_value_str[8] = {0};
			parse_uart_value(uart_current_msg, sensitivity_value_str, &index);
			sensitivity_level = (uint8_t)atoi(sensitivity_value_str);
			currentState = CHANGE_SENSITIVITY;
        }

        else if (strcmp(cmd_str, "acqChange") == 0)
		{
			// Get acquisition change parameter
        	uint8_t index = comma_index; // Value starts after comma
        	char acq_param_str[8] = {0};
        	uint8_t acq_cmd_value =0;
        	parse_uart_value(uart_current_msg, acq_param_str, &index);
        	acq_cmd_value = (uint8_t)atoi(acq_param_str);
        	if (acq_cmd_value == RESET_DEVICE)
        	{
        		acq_cmd_param = RESET_DEVICE;
        	}
        	else if (acq_cmd_value == TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION)
        	{
        		acq_cmd_param = TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION;
        	}
        	else if (acq_cmd_value == TAKE_DISTANCE_MEASUREMENT_NO_BIAS_CORRECTION)
        	{
        	    acq_cmd_param = TAKE_DISTANCE_MEASUREMENT_NO_BIAS_CORRECTION;
        	}

        	currentState = ACQ_CHANGE;
		}

        else
        {
            // Unknown command
            char resp[64];
            int len = snprintf(resp, sizeof(resp), "Error: Unknown command '%s'\r\n", cmd_str);
            if (len > 0)
            {
                HAL_UART_Transmit(&huart3, (uint8_t *)resp, (uint16_t)len, HAL_MAX_DELAY);
            }
        }
    }


    // Clear DMA buffer and restart reception
    memset(UART3_rxBuffer, 0, UART_DMA_RX_SIZE);
    HAL_UART_Receive_DMA(&huart3, UART3_rxBuffer, UART_DMA_RX_SIZE);

}









/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
