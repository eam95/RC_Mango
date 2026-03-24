/*
 * GarminLidarV3_Config.c
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


void config_ACQ_COMMAND_reg(uint8_t command, uint8_t print)
{
	GarLiteV3_i2c_write(ACQ_COMMAND, &command, 1);
//	HAL_Delay(1);
	if (print == 1)
	{
		print_config_ACQ_COMMAND_reg();
	}
}



uint8_t read_ACQ_COMMAND_reg(void)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(ACQ_COMMAND, &regval, 1);
	return regval;
}



void print_config_ACQ_COMMAND_reg(void)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(ACQ_COMMAND, &regval, 1);

	char buf[128];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- ACQ_COMMAND -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 1: raw value
	snprintf(buf, sizeof(buf), "ACQ_COMMAND (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 2: Command description
	switch (regval)
	{
		case RESET_DEVICE:
			snprintf(buf, sizeof(buf), "Device Reset\r\n");
			break;
		case TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION:
			snprintf(buf, sizeof(buf), "Measure Distance W Bias\r\n");
			break;
		case TAKE_DISTANCE_MEASUREMENT_NO_BIAS_CORRECTION:
			snprintf(buf, sizeof(buf), "Measure Distance WO Bias\r\n");
			break;
		default:
			snprintf(buf, sizeof(buf), "Command: Unknown command\r\n");
			break;
	}
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line for readability
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



uint8_t read_STATUS_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(STATUS, &regval, 1);
//	HAL_Delay(1);
	if (print == 1)
	{
		print_STATUS_reg(regval);
	}
	return regval;
}



void print_STATUS_reg(uint8_t regval)
{
	char buf[128];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- STATUS -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 1: raw value
	snprintf(buf, sizeof(buf), "STATUS (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 2: Process Error Flag
	if (regval & PROCESS_ERROR_FLAG)
		snprintf(buf, sizeof(buf), "System error detected during measurement\r\n");
	else
		snprintf(buf, sizeof(buf), "No error detected\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 3: Health Flag
	if (regval & HEALTH_FLAG)
		snprintf(buf, sizeof(buf), "Ref & receiver bias are operational\r\n");
	else
		snprintf(buf, sizeof(buf), "Error detected\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 4: Secondary Return Flag
	if (regval & SECONDARY_RETURN_FLAG)
		snprintf(buf, sizeof(buf), "Secondary return detected in correlation record\r\n");
	else
		snprintf(buf, sizeof(buf), "No secondary return\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 5: Invalid Signal Flag
	if (regval & INVALID_SIGNAL_FLAG)
		snprintf(buf, sizeof(buf), "Peak not detected in correlation method, measurement in invalid\r\n");
	else
		snprintf(buf, sizeof(buf), "Peak detected\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 6: Signal Overflow Flag
	if (regval & SIGNAL_OVERFLOW_FLAG)
		snprintf(buf, sizeof(buf), "Signal overflow occurred during acquisition\r\n");
	else
		snprintf(buf, sizeof(buf), "No signal overflow\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 7: Reference Overflow Flag
	if (regval & REFERENCE_OVERFLOW_FLAG)
		snprintf(buf, sizeof(buf), "Reference overflow occurred during acquisition\r\n");
	else
		snprintf(buf, sizeof(buf), "No reference overflow\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 8: Busy Flag
	if (regval & BUSY_FLAG)
		snprintf(buf, sizeof(buf), "Device is busy performing a measurement\r\n");
	else
		snprintf(buf, sizeof(buf), "Device is idle\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line for readability
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}




void config_SIG_COUNT_VAL_reg(uint8_t sigCountVal, uint8_t print)
{
	GarLiteV3_i2c_write(SIG_COUNT_VAL, &sigCountVal, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		uint8_t regval = 0;
		GarLiteV3_i2c_read(SIG_COUNT_VAL, &regval, 1);
		// Read back register from device to detect actual settings
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- SIG_COUNT_VAL -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "SIG_COUNT_VAL (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Signal Count Value
		snprintf(buf, sizeof(buf), "Signal Count Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



uint8_t read_SIG_COUNT_VAL_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(SIG_COUNT_VAL, &regval, 1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- SIG_COUNT_VAL -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "SIG_COUNT_VAL (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Signal Count Value
		snprintf(buf, sizeof(buf), "Signal Count Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



void config_ACQ_CONFIG_REG(uint8_t RefProcess, uint8_t delayMode, uint8_t RefFilter, uint8_t QuickTermination, uint8_t AcqCount, uint8_t PinFunc ,uint8_t print)
{
	uint8_t config = 0;
	config = RefProcess | delayMode | RefFilter | QuickTermination | AcqCount | PinFunc;
	GarLiteV3_i2c_write(ACQ_CONFIG_REG, &config, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_ACQ_CONFIG_REG();
	}
}



void print_config_ACQ_CONFIG_REG(void)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(ACQ_CONFIG_REG, &regval, 1);

	char buf[256];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- ACQ_CONFIG_REG -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 1: raw value
	snprintf(buf, sizeof(buf), "ACQ_CONFIG_REG (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 2: Reference Process
	if (regval & ENABLE_REFERENCE_PROCESS)
		snprintf(buf, sizeof(buf), "Reference Process: Enabled\r\n");
	else
		snprintf(buf, sizeof(buf), "Reference Process: Disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 3: Measure Delay
	if (regval & ENABLE_MEASURE_DELAY)
		snprintf(buf, sizeof(buf), "Measure Delay: Enabled\r\n");
	else
		snprintf(buf, sizeof(buf), "Measure Delay: Disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 4: Reference Filter
	if (regval & ENABLE_REFERENCE_FILTER)
		snprintf(buf, sizeof(buf), "Reference Filter: Enabled\r\n");
	else
		snprintf(buf, sizeof(buf), "Reference Filter: Disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 5: Quick Termination
	if (regval & ENABLE_QUICK_TERMINATION)
		snprintf(buf, sizeof(buf), "Quick Termination: Enabled\r\n");
	else
		snprintf(buf, sizeof(buf), "Quick Termination: Disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 6: Use Reference Acquisition Count
	if (regval & ENABLE_REFERENCE_ACQ_COUNT)
		snprintf(buf, sizeof(buf), "Use Reference Acquisition Count: Enabled\r\n");
	else
		snprintf(buf, sizeof(buf), "Use Reference Acquisition Count: Disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	// Line 7: Pin Function Control
	switch (regval & 0x03)
	{
		case DEFAULT_PWM_Mode:
			snprintf(buf, sizeof(buf), "Pin Function Control: Default PWM Mode\r\n");
			break;
		case STATUS_OUTPUT_MODE:
			snprintf(buf, sizeof(buf), "Pin Function Control: Status Output Mode\r\n");
			break;
		case FIXED_DELAY_PWM_MODE:
			snprintf(buf, sizeof(buf), "Pin Function Control: Fixed Delay PWM Mode\r\n");
			break;
		case OSCILLATOR_OUTPUT_MODE:
			snprintf(buf, sizeof(buf), "Pin Function Control: Oscillator Output Mode\r\n");
			break;
		default:
			snprintf(buf, sizeof(buf), "Pin Function Control: Unknown mode\r\n");
			break;
	}
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	// Footer blank line for readability
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



uint8_t read_VELOCITY_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(VELOCITY, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- VELOCITY -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "VELOCITY (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Velocity Value
		snprintf(buf, sizeof(buf), "Velocity Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_PEAK_CORR_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(PEAK_CORR, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- PEAK_CORR -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "PEAK_CORR (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Peak Correlation Value
		snprintf(buf, sizeof(buf), "Peak Correlation Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_NOISE_PEAK_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(NOISE_PEAK, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- NOISE_PEAK -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "NOISE_PEAK (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Noise Peak Value
		snprintf(buf, sizeof(buf), "Noise Peak Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_SIGNAL_STRENGTH_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(SIGNAL_STRENGTH, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- SIGNAL_STRENGTH -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "SIGNAL_STRENGTH (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Signal Strength Value
		snprintf(buf, sizeof(buf), "Signal Strength Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_FULL_DELAY_HIGH_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(FULL_DELAY_HIGH, &regval, 1);
//	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- FULL_DELAY_HIGH -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "FULL_DELAY_HIGH (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Full Delay High Value
		snprintf(buf, sizeof(buf), "Full Delay High Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_FULL_DELAY_LOW_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(FULL_DELAY_LOW, &regval, 1);
//	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- FULL_DELAY_LOW -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "FULL_DELAY_LOW (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Full Delay Low Value
		snprintf(buf, sizeof(buf), "Full Delay Low Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}

void config_OUTER_LOOP_COUNT_reg(uint8_t count, uint8_t print)
{
	if (count == SINGLE_MEASUREMENT || count == CONT_MEASUREMENT)
	{
		GarLiteV3_i2c_write(OUTER_LOOP_COUNT, &count, 1);
		HAL_Delay(1);
		if (print == 1)
		{
			char buf[64];
			uint8_t regval = 0;
			// Read back register to confirm setting
			GarLiteV3_i2c_read(OUTER_LOOP_COUNT, &regval, 1);
			// Divider/header
			snprintf(buf, sizeof(buf), "----- OUTER_LOOP_COUNT -----\r\n");
			HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

			// Line 1: raw value
			snprintf(buf, sizeof(buf), "OUTER_LOOP_COUNT set to: 0x%02X\r\n", regval);
			HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

			// Line 2: Description
			if (regval == SINGLE_MEASUREMENT)
				snprintf(buf, sizeof(buf), "Configured for Single Measurement Mode\r\n");
			else
				snprintf(buf, sizeof(buf), "Configured for Continuous Measurement Mode\r\n");
			HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

			// Footer blank line for readability
			snprintf(buf, sizeof(buf), "\r\n");
			HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		}
	}
	else
	{
		GarLiteV3_i2c_write(OUTER_LOOP_COUNT, &count, 1);
		HAL_Delay(1);
		if (print == 1)
		{
			char buf[128];
			uint8_t regval = 0;
			// Read back register to confirm setting
			GarLiteV3_i2c_read(OUTER_LOOP_COUNT, &regval, 1);
			// Divider/header
			snprintf(buf, sizeof(buf), "----- OUTER_LOOP_COUNT -----\r\n");
			HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

			// Line 1: raw value
			snprintf(buf, sizeof(buf), "OUTER_LOOP_COUNT set to: 0x%02X\r\n", regval);
			HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

			// Line 2: Description
			snprintf(buf, sizeof(buf), "Configured for Burst Measurement Count: %d\r\n", regval);
			HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

			// Footer blank line for readability
			snprintf(buf, sizeof(buf), "\r\n");
			HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		}
	}
}



uint8_t read_REF_COUNT_VAL_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(REF_COUNT_VAL, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- REF_COUNT_VAL -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "REF_COUNT_VAL (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Reference Count Value
		snprintf(buf, sizeof(buf), "Reference Count Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_LAST_DELAY_HIGH_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(LAST_DELAY_HIGH, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- LAST_DELAY_HIGH -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "LAST_DELAY_HIGH (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Last Delay High Value
		snprintf(buf, sizeof(buf), "Last Delay High Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_LAST_DELAY_LOW_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(LAST_DELAY_LOW, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- LAST_DELAY_LOW -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "LAST_DELAY_LOW (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Last Delay Low Value
		snprintf(buf, sizeof(buf), "Last Delay Low Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



void read_UNIT_ID_HIGH_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(UNIT_ID_HIGH, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- UNIT_ID_HIGH -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "UNIT_ID_HIGH (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Unit ID High Value
		snprintf(buf, sizeof(buf), "Unit ID High Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



void read_UNIT_ID_LOW_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(UNIT_ID_LOW, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- UNIT_ID_LOW -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "UNIT_ID_LOW (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Unit ID Low Value
		snprintf(buf, sizeof(buf), "Unit ID Low Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



void read_I2C_ID_HIGH_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(I2C_ID_HIGH, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- I2C_ID_HIGH -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "I2C_ID_HIGH (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: I2C ID High Value
		snprintf(buf, sizeof(buf), "I2C ID High Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



void read_I2C_ID_LOW_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(I2C_ID_LOW, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- I2C_ID_LOW -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "I2C_ID_LOW (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: I2C ID Low Value
		snprintf(buf, sizeof(buf), "I2C ID Low Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



void read_I2C_SEC_ADDR_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(I2C_SEC_ADDR, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- I2C_SEC_ADDR -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "I2C_SEC_ADDR (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: I2C Secondary Address Value
		snprintf(buf, sizeof(buf), "I2C Secondary Address Value: 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



void config_THRESHOLD_BYPASS_reg(uint8_t sensitivity, uint8_t print)
{
	GarLiteV3_i2c_write(THRESHOLD_BYPASS, &sensitivity, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		uint8_t regval = 0;
		// Read back register to confirm setting
		GarLiteV3_i2c_read(THRESHOLD_BYPASS, &regval, 1);
		// Divider/header
		snprintf(buf, sizeof(buf), "----- THRESHOLD_BYPASS -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "THRESHOLD_BYPASS set to: 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Description
		snprintf(buf, sizeof(buf), "Threshold Bypass Sensitivity set to: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



void config_COMMAND_reg(uint8_t command, uint8_t print)
{
	GarLiteV3_i2c_write(COMMAND, &command, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		uint8_t regval = 0;
		// Read back register to confirm setting
		GarLiteV3_i2c_read(COMMAND, &regval, 1);
		// Divider/header
		snprintf(buf, sizeof(buf), "----- COMMAND -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "COMMAND set to: 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Description
		switch (regval)
		{
			case TEST_MODE_ENABLE:
				snprintf(buf, sizeof(buf), "Allowed download of correlation record\r\n");
				break;
			case TEST_MODE_DISABLE:
				snprintf(buf, sizeof(buf), "Resume normal operation\r\n");
				break;
			default:
				snprintf(buf, sizeof(buf), "Command: Unknown command\r\n");
				break;
		}
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



void config_MEASURE_DELAY_reg(uint8_t delay, uint8_t print)
{
	GarLiteV3_i2c_write(MEASURE_DELAY, &delay, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		uint8_t regval = 0;
		// Read back register to confirm setting
		GarLiteV3_i2c_read(MEASURE_DELAY, &regval, 1);
		// Divider/header
		snprintf(buf, sizeof(buf), "----- MEASURE_DELAY -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "MEASURE_DELAY set to: 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Description
		snprintf(buf, sizeof(buf), "Measurement Delay set to: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}



uint8_t read_PEAK_BCK_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(PEAK_BCK, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- PEAK_BCK -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "PEAK_BCK (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Peak Background Value
		snprintf(buf, sizeof(buf), "Peak Background Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_CORR_DATA_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(CORR_DATA, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- CORR_DATA -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "CORR_DATA (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Correlation Data Value
		snprintf(buf, sizeof(buf), "Correlation Data Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



uint8_t read_CORR_DATA_SIGN_reg(uint8_t print)
{
	uint8_t regval = 0;
	GarLiteV3_i2c_read(CORR_DATA_SIGN, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		// Divider/header
		snprintf(buf, sizeof(buf), "----- CORR_DATA_SIGN -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "CORR_DATA_SIGN (read) = 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Correlation Data Sign Value
		snprintf(buf, sizeof(buf), "Correlation Data Sign Value: %d\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	return regval;
}



void config_ACQ_SETTINGS_reg(uint8_t bankAccess, uint8_t print)
{
	GarLiteV3_i2c_write(ACQ_SETTINGS, &bankAccess, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		uint8_t regval = 0;
		// Read back register to confirm setting
		GarLiteV3_i2c_read(ACQ_SETTINGS, &regval, 1);
		// Divider/header
		snprintf(buf, sizeof(buf), "----- ACQ_SETTINGS -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "ACQ_SETTINGS set to: 0x%02X\r\n", regval);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Description
		if (regval & ACCESS_CORR_MEM_BANK)
			snprintf(buf, sizeof(buf), "Acquisition Bank Access: Enabled\r\n");
		else
			snprintf(buf, sizeof(buf), "Acquisition Bank Access: Disabled\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}

}



void config_POWER_CONTROL_reg(uint8_t wakeMode, uint8_t receiverCircuit, uint8_t print)
{
	uint8_t regval = (wakeMode | receiverCircuit);
	GarLiteV3_i2c_write(POWER_CONTROL, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		char buf[64];
		uint8_t readback = 0;
		// Read back register to confirm setting
		GarLiteV3_i2c_read(POWER_CONTROL, &readback, 1);
		// Divider/header
		snprintf(buf, sizeof(buf), "----- POWER_CONTROL -----\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 1: raw value
		snprintf(buf, sizeof(buf), "POWER_CONTROL set to: 0x%02X\r\n", readback);
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 2: Wake Mode
		if (readback & DEVICE_SLEEP)
			snprintf(buf, sizeof(buf), "Device sleep, wakes on i2c transaction\r\n");
		else
			snprintf(buf, sizeof(buf), "Device Awake\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Line 3: Receiver Circuit
		if (readback & ENABLE_RECEIVER_CIRCUIT)
			snprintf(buf, sizeof(buf), "Receiver Circuit: Enabled\r\n");
		else
			snprintf(buf, sizeof(buf), "Receiver Circuit: Disabled\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

		// Footer blank line for readability
		snprintf(buf, sizeof(buf), "\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
}











