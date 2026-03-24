/*
 * GarminLidarLiteV3.c
 *
 *  Created on: Nov 10, 2025
 *      Author: erick
 */
#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "GarminLidarLiteV3.h"
#include "main.h"
#include "delay.h"

extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart3;
extern volatile uint8_t acq_cmd_param; // reference global from main.c

void GarLiteV3_i2c_write(uint8_t reg, uint8_t *buf, uint16_t len)
{
//	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	// Use HAL_I2C_Mem_Write to write 'len' bytes starting at register 'reg'.
	if (HAL_I2C_Mem_Write(&hi2c2, LIDARLITEV3_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY) != HAL_OK)
	{
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		HAL_UART_Transmit(&huart3, (uint8_t *)"Garmin Lidar I2C Command Transmit Error\r\n", strlen("Garmin Lidar I2C Command Transmit Error\r\n"), HAL_MAX_DELAY);
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	}
//	else
//	{
////		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
//	}
}



void GarLiteV3_i2c_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
//	HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
	// Use HAL_I2C_Mem_Read to read 'len' bytes starting at register 'reg'.
	if (HAL_I2C_Mem_Read(&hi2c2, LIDARLITEV3_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY) != HAL_OK)
	{
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		HAL_UART_Transmit(&huart3,  (uint8_t *)"Garmin Lidar I2C Command Transmit Error\r\n", strlen("Garmin Lidar I2C Command Transmit Error\r\n"), HAL_MAX_DELAY);
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	}
//	else
//	{
////		HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
//	}
}


void GarLiteV3_Init(void)
{
	// Initialization sequence for Garmin Lidar Lite V3 can be added here if needed.
	// Simple setup from datasheet.
	config_ACQ_COMMAND_reg(RESET_DEVICE,1);

	// 1. Measure with bias correction
	uint8_t sensititityLevel = 0; // Example sensitivity level
	config_ACQ_COMMAND_reg(TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION,1);
	HAL_Delay(10);
	config_THRESHOLD_BYPASS_reg(TH_SENSITIVITY_LEVEL(sensititityLevel),1);
	HAL_Delay(10);


}


void simple_measurement(uint16_t *distanceCm, uint8_t print)
{
	// Start a simple distance measurement with bias correction
	  uint8_t statusRegVal = 0;
	  uint8_t fullDelayHigh = 0;
	  uint8_t fullDelayLow = 0;
	  uint32_t timeDelay = adjusteTimeForTicks(200);
	  HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	  statusRegVal = read_STATUS_reg(0);

	  if ((statusRegVal & BUSY_FLAG) == 0)
	  {
		  // Measurement complete
		  HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		  fullDelayHigh = read_FULL_DELAY_HIGH_reg(0);
		  fullDelayLow = read_FULL_DELAY_LOW_reg(0);
		  *distanceCm = ((uint16_t)fullDelayHigh << 8) | fullDelayLow;
		  if (print == 1)
		  {
			  char buf[64];
			  snprintf(buf, sizeof(buf), "Distance Measurement Complete: %d cm\r\n", *distanceCm);
			  HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		  }

		  // Start a new measurement
		  config_ACQ_COMMAND_reg(acq_cmd_param, 0);
		  delay_us(timeDelay);

	  }
}



void acq_count_measurements(uint16_t *distanceCm, uint8_t print)
{
    uint8_t statusRegVal;
    uint32_t timeDelay = adjusteTimeForTicks(200);

    // 1. Start new measurement
    config_ACQ_COMMAND_reg(0x04, 0);

    // 2. Wait until BUSY = 1 (measurement started)
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    do
    {
        statusRegVal = read_STATUS_reg(0);
    } while ((statusRegVal & BUSY_FLAG) == 0);

    // 3. Wait until BUSY = 0 (measurement finished)
    do
    {
        statusRegVal = read_STATUS_reg(0);
    } while ((statusRegVal & BUSY_FLAG) != 0);
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
//    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

    // 4. Read result
    uint8_t high = read_FULL_DELAY_HIGH_reg(0);
    uint8_t low  = read_FULL_DELAY_LOW_reg(0);

    *distanceCm = ((uint16_t)high << 8) | low;
//    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    delay_us(timeDelay);

    if (print == 1)
    {
        char buf[64];
        snprintf(buf, sizeof(buf),
                 "Distance Measurement: %d cm\r\n", *distanceCm);
        HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
    }
}




void burst_measurements(uint16_t *distanceArray, uint16_t numBurst, uint8_t print)
{
    uint8_t statusRegVal;
    char buf[64];

    if (print == 1)
    {
        snprintf(buf, sizeof(buf), "Starting burst of %u measurements...\r\n", numBurst);
        HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
    }

    // Read each measurement in the burst
    for (uint16_t i = 0; i < numBurst; i++)
    {
        // Wait for measurement to complete (BUSY flag clears)
        HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
        do
        {
            statusRegVal = read_STATUS_reg(0);
        } while ((statusRegVal & BUSY_FLAG) != 0);
        HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

        // Read the distance
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
        uint8_t high = read_FULL_DELAY_HIGH_reg(0);
        uint8_t low  = read_FULL_DELAY_LOW_reg(0);
        distanceArray[i] = ((uint16_t)high << 8) | low;
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

        if (print == 1)
        {
            snprintf(buf, sizeof(buf), "Burst %u: %u cm\r\n", i+1, distanceArray[i]);
            HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
        }

        // LIDAR automatically triggers next measurement after MEASURE_DELAY
        // NO need to call config_ACQ_COMMAND_reg() again!
    }

    if (print == 1)
    {
        snprintf(buf, sizeof(buf), "Burst complete!\r\n");
        HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
    }
}
