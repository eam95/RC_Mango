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
#include "LIS3DH.h"
#include "stm32h7xx_hal.h"
#include "delay.h" // Include delay.h for delay functions
#include "commandReader.h"
#include "NRF24_conf.h"
#include "NRF24_reg_addresses.h"
#include "NRF24.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_DMA_RX_SIZE 32
#define PLD_S 32

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim24;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
///////////////////// State Machines  ////////////////////////////////////////////////////////
typedef enum
{
	WAIT_STATE,
	FORWARD_STATE,
	BACKWARD_STATE,
	BRAKE_STATE,
	COAST_STATE,
	GET_SENSOR_DATA_STATE,
	SET_PID_STATE
} StateType;
StateType currentState = WAIT_STATE;
StateType nextState = WAIT_STATE;

typedef enum
{
	RX_STATE,
	TX_STATE
} CommModeType;
CommModeType currentCommMode = RX_STATE;

///////////////////// Structures To use as Global Variables   ////////////////////////////////////////////////////////

struct DataToTransmit
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

struct DataToReceive
{
	// Disregarding the cmd, all the variables byte size should add up to 14-16bytes
	uint8_t cmd[PLD_S]; // The command is received as a string and will be parsed to determine the action and parameters to store
	// under the DataToReceive structure.
	int32_t pwm_value; // Temporarily will use pwm value, for testing the car until the speed can be determined after figure out the actual speed of the RC car.
	int16_t setSpeed; // Will grab the speed value received from the command and store it here, which will be used to set the PWM duty cycle for motor control.
	int16_t Pset; // PID proportional gain
	int16_t Iset; // PID integral gain
	int16_t Dset; // PID derivative gain
	int32_t sampleTime; // Sampling time for the PID controller, which will determine how often the control loop runs and updates the motor commands based on the sensor feedback.
};

struct TimeTracking
{
	uint32_t seconds_counter; // This will keep track of the total elapsed time in seconds since the system started, which can be useful for timestamping data and events.
	uint32_t set_timer_period; // This variable will store the timer period in ticks, which is used to configure the timer interrupt frequency for tasks like sensor reading and control loop execution.
	uint32_t stamped_time; // This variable will store the timestamp of the last sensor reading or control loop execution, which can be used to calculate elapsed time and ensure consistent timing for the control algorithm.
	uint32_t milliAdder; // This variable will add a certain number of milliseconds to the stamped_time to determine when the next sensor reading or control loop execution should occur, allowing for precise timing control in the system.
	uint32_t timeAccumulator; // This variable will accumulate the elapsed time in milliseconds.
};

///////////////////// Variable Declarations  ////////////////////////////////////////////////////////

////// UART Declarations  //////////////////////////////////////
/// will not be used unless there is some troubleshooting needed.
// For UART message processing
char uart_current_msg[UART_DMA_RX_SIZE];
// UART DMA Buffer
uint8_t UART3_rxBuffer[UART_DMA_RX_SIZE]; // DMA buffer



////// Time Tracking Declarations  //////////////////////////////////////
struct TimeTracking Tset =
{
    .seconds_counter = 0,
    .set_timer_period = 10000,
	.stamped_time = 0,
	.milliAdder = 0,
	.timeAccumulator = 0
};
// Rate of ticks is 1MHz with prescalar of 100 (assuming 100MHz clock)
// 1ticks = 1us



//////  LIDAR Declarations  //////////////////////////////////////
// Sensitivity level
volatile uint8_t sensitivity_level = 100;
volatile uint8_t acq_cmd_param = TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION;


//////  Commands Declarations  //////////////////////////////////////
struct DataToTransmit Transmit = {0};  // This initializes all members to 0
struct DataToReceive Receive = {0};    // This initializes all members to 0


//////  NRF24L01 Declarations  //////////////////////////////////////
uint8_t tx_addr[5] = {0x45, 0x55, 0x67, 0x10, 0x21};

volatile int pwm_value = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM24_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void nrf24_setup(void);

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
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM24_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  lis3dh_init();
  GarLiteV3_Init();
  HAL_UART_Receive_DMA(&huart3, UART3_rxBuffer, UART_DMA_RX_SIZE);
  // Start TIM3 with interrupt
  HAL_TIM_Base_Start_IT(&htim3);
  // Start TIM2 with interrupt
//  HAL_TIM_Base_Start_IT(&htim2);
  // Initialized NRF24L01
  nrf24_setup();
  nrf24_listen();


  // Use absolute compare (tick) values directly instead of percentage
  // These are timer ticks (must be <= htim1.Init.Period)
  uint32_t pulse_ch3 = 25000U; // channel 1
  uint32_t pulse_ch4 = 25000U; // channel 2
//  char msg[50];

  // Set up time Adder for timestamping and timing control
  Tset.milliAdder = Tset.set_timer_period / 1000;


  /* Sanity checks: ensure values do not exceed ARR (which is htim1.Init.Period) */
  if (pulse_ch3 > (uint32_t)htim1.Init.Period)
  {
    Error_Handler();
  }
  if (pulse_ch4 > (uint32_t)htim1.Init.Period)
  {
    Error_Handler();
  }

  /* Start PWM channels */
  if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }


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

		  case FORWARD_STATE:

			  pulse_ch3 = Receive.pwm_value;
			  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pulse_ch3);
			  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
//			  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
//			  strcpy(msg, "State: Forward\r\n");
//			  HAL_UART_Transmit(&huart3, (char *)&msg, strlen(msg), HAL_MAX_DELAY); // Transmit a message for testingg
			  break;

		  case BACKWARD_STATE:
			  pulse_ch4 = Receive.pwm_value;
			  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
			  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pulse_ch4);
//			  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
			  break;

		  case BRAKE_STATE:
			  pulse_ch3 = Receive.pwm_value;
			  pulse_ch4 = Receive.pwm_value;
			  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pulse_ch3);
			  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pulse_ch4);
//			  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
			  break;

		  case COAST_STATE:
			  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
			  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
//			  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
//			  strcpy(msg, "State: Coast\r\n");
//			  HAL_UART_Transmit(&huart3, (char *)&msg, strlen(msg), HAL_MAX_DELAY); // Transmit a message for testing
			  currentState = WAIT_STATE; // Transition back to wait state after coasting
			  break;

		  case SET_PID_STATE:
//			  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
			  // Will set up PID parameters later on, after performing enough test.
			  break;

		  case GET_SENSOR_DATA_STATE:
//			  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
			  // Get Accelerometer data for now.
//			  read_XYZ_Acceleration(&Transmit.a_x, &Transmit.a_y, &Transmit.a_z); /// Return 16 bit raw data, need to convert to mg in the future if needed. For now just transmit the raw data.
			  measure_XYZ_Acceleration(&Transmit.a_x, &Transmit.a_y, &Transmit.a_z);
			  simple_measurement(&Transmit.distance_cm,0);
			  // Packet the data to transmit through NRF24L01
			  Tset.stamped_time = Tset.stamped_time + Tset.milliAdder; // Get the current time in milliseconds
//			  uint16_t len = snprintf((char *)Transmit.cmd, sizeof(Transmit.cmd), "%lu,%u\r\n,%u\r\n,%u\r\n,%u\r\n",Tset.stamped_time, Transmit.distance_cm, Transmit.a_x, Transmit.a_y, Transmit.a_z);
			  uint16_t len = snprintf((char *)Transmit.cmd, sizeof(Transmit.cmd), "%lu,%u,%ld,%ld,%ld\r\n",
			                          Tset.stamped_time, Transmit.distance_cm, Transmit.a_x, Transmit.a_y, Transmit.a_z);

		      if (len > 0)
			  {
				  HAL_UART_Transmit(&huart3, (uint8_t *)Transmit.cmd, (uint16_t)len, HAL_MAX_DELAY);
			  }
		      nrf24_transmit(Transmit.cmd, PLD_S); // Transmit the data through NRF24L01
			  currentState = nextState; // Transition back to wait state after getting sensor data
			  break;

		  default:
			  // Should never reach here
			  Error_Handler();
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00C042E4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  htim2.Init.Period = 10000;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 250-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = Tset.set_timer_period-1; // Set the period based on the desired timer interrupt frequency (e.g., 10ms)
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
//  htim3.Init.Period = Tset.set_timer_period-1;
  /* USER CODE END TIM3_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, ce_Pin|csn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : ce_Pin csn_Pin */
  GPIO_InitStruct.Pin = ce_Pin|csn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_GREEN_Pin LED_RED_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin|LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : IQR_RF_Pin */
  GPIO_InitStruct.Pin = IQR_RF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IQR_RF_GPIO_Port, &GPIO_InitStruct);

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

	if(htim->Instance == TIM3)
	{
		// Check if the interrupt is from TIM3
		char ReceiveCmd[PLD_S];
		switch (currentCommMode)
		{
		case RX_STATE:
		    // Set YELLOW high, GREEN low to indicate RX state
		    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_SET);
		    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

		    if (nrf24_data_available())
		    {
		        nrf24_receive(Receive.cmd, sizeof(Receive.cmd));
		        nrf24_clear_rx_dr();
		        strncpy(ReceiveCmd, (char *)Receive.cmd, PLD_S - 1);
		        ReceiveCmd[PLD_S - 1] = '\0';

		        char cmd_str[32] = {0};
		        char pwm_str[8] = {0};
		        uint8_t comma_index = 0;
		        parse_uart_command(ReceiveCmd, cmd_str, &comma_index);

		        if (strcmp(cmd_str, "S") == 0)
		        {
		        }
		        else if (strcmp(cmd_str, "E") == 0)
		        {
		        }
		        else if (strcmp(cmd_str, "F") == 0)
		        {
		            uint8_t index = comma_index;
		            parse_uart_value(ReceiveCmd, pwm_str, &index);
		            Receive.pwm_value = (uint16_t)atoi(pwm_str);
		            currentState = FORWARD_STATE;
		        }
		        else if (strcmp(cmd_str, "R") == 0)
		        {
		            uint8_t index = comma_index;
		            parse_uart_value(ReceiveCmd, pwm_str, &index);
		            Receive.pwm_value = (uint16_t)atoi(pwm_str);
		            currentState = BACKWARD_STATE;
		        }
		        else if (strcmp(cmd_str, "B") == 0)
		        {
		            Receive.pwm_value = 25000;
		            currentState = BRAKE_STATE;
		        }
		        else if (strcmp(cmd_str, "C") == 0)
		        {
		            currentState = COAST_STATE;
		        }
		        else if (strcmp(cmd_str, "PID") == 0)
		        {
		            uint8_t index = comma_index;
		            char P_str[8] = {0};
		            char I_str[8] = {0};
		            char D_str[8] = {0};
		            parse_uart_value(ReceiveCmd, P_str, &index);
		            Receive.Pset = (uint16_t)atoi(P_str);
		            parse_uart_value(ReceiveCmd, I_str, &index);
		            Receive.Iset = (uint16_t)atoi(I_str);
		            parse_uart_value(ReceiveCmd, D_str, &index);
		            Receive.Dset = (uint16_t)atoi(D_str);
		            currentState = SET_PID_STATE;
		        }
		        currentCommMode = TX_STATE;
		        nrf24_stop_listen();
		    }
		    else
		    {
		        currentCommMode = TX_STATE;
		        nrf24_stop_listen();
		    }
		    break;

		case TX_STATE:
		    // Set GREEN high, YELLOW low to indicate TX state
		    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
		    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_RESET);

		    if (currentState != WAIT_STATE)
		    {
		        strncpy(ReceiveCmd, (char *)Receive.cmd, PLD_S - 1);
		        ReceiveCmd[PLD_S - 1] = '\0';
		        char cmd_str[32] = {0};
		        uint8_t comma_index = 0;
		        parse_uart_command(ReceiveCmd, cmd_str, &comma_index);
		        currentState = GET_SENSOR_DATA_STATE;

		        if (strcmp(cmd_str, "F") == 0)
		        {
		            nextState = FORWARD_STATE;
		        }
		        else if (strcmp(cmd_str, "R") == 0)
		        {
		            nextState = BACKWARD_STATE;
		        }
		        else if (strcmp(cmd_str, "C") == 0)
		        {
		            currentState = COAST_STATE;
		        }
		    }

		    currentCommMode = RX_STATE;
		    nrf24_listen();
		    break;

			default:
				break;
		}

	}
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
