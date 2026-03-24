/*
 * LIS3DH.c
 *
 *  Created on: Oct 17, 2025
 *      Author: erick
 */
#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "LIS3DH.h"
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart3;


void lis3dh_i2c_write(uint8_t reg, uint8_t *buf, uint16_t len)
{
	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	// Use HAL_I2C_Mem_Write to write 'len' bytes starting at register 'reg'.
	if (HAL_I2C_Mem_Write(&hi2c1, LIS3DH_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY) != HAL_OK)
	{
		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		HAL_UART_Transmit(&huart3, (uint8_t *)"LIS3DH I2C Command Transmit Error\r\n", strlen("LIS3DH I2C Command Transmit Error\r\n"), HAL_MAX_DELAY);
		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	}
	else
	{
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	}
}


void lis3dh_i2c_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
	HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
	// Use HAL_I2C_Mem_Read to read 'len' bytes starting at register 'reg'.
	if (HAL_I2C_Mem_Read(&hi2c1, LIS3DH_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY) != HAL_OK)
	{
		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		HAL_UART_Transmit(&huart3, (uint8_t *)"LIS3DH I2C Command Receive Error\r\n", strlen("LIS3DH I2C Command Receive Error\r\n"), HAL_MAX_DELAY);
		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	}
	else
	{
		HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
	}
}


void lis3dh_init(void)
{
	// Enable temperature sensor and basic configuration
	config_temp_cfg_reg(DISABLE_TEMPERATURE_SENSOR, 1);
	HAL_Delay(10);
	config_ctrl_reg1(ODR_10HZ, ENABLE_NORMAL_POWER_MODE, ENABLE_ALL_AXES, 1);
	HAL_Delay(10);
	config_ctrl_reg2(HPM_NORMAL_MODE_After_RESET, HPCF_CUTOFF_2,
				DISABLE_FILTERED_DATA_SELECTION, DISABLE_HPF_CLICK,
				DISABLE_HPF_AOI_INT2, DISABLE_HPF_AOI_INT1, 1);
	HAL_Delay(10);
	// Enable INT1 FIFO watermark so INT pin asserts when FIFO >= watermark
	config_ctrl_reg3(DISABLE_INT1_CLICK, DISABLE_INT1_AOI1, DISABLE_INT1_AOI2,
				DISABLE_INT1_DATA_READY1, DISABLE_INT1_DATA_READY2,
				DISABLE_INT1_FIFO_WATERMARK, DISABLE_INT1_OVERRUN, 1);
	HAL_Delay(10);
	config_ctrl_reg4(ENABLE_BLOCK_DATA_UPDATE, ENABLE_LITTLE_ENDIAN, FS_2G,
				DISABLE_HIGH_RESOLUTION_MODE, DISABLE_SELF_TEST, ENABLE_3_WIRE_SPI, 1);
	HAL_Delay(10);
	// Enable FIFO in CTRL_REG5
	config_ctrl_reg5(NORMAL_OPERATION, DISABLE_FIFO,
				DISABLE_LATCHED_INT1, DISABLE_4D_INT1, 1);
	HAL_Delay(10);
	// Configure FIFO mode and set watermark to 10 samples
//	config_fifo_ctrl_reg(FIFO_MODE, ENABLE_TRIGGER_INT1, FIFO_WATERMARK_LEVEL(10), 1);
//	HAL_Delay(10);
	config_ctrl_reg6(DISABLE_INT2_CLICK, DISABLE_INT2_AOI1, DISABLE_INT2_AOI2,
				DISABLE_BOOT_INT2, DISABLE_INT2_ACTIVITY, DISABLE_INT_Polarity, 1);
	HAL_Delay(10);
	// Keep FIFO enabled (do not disable here)
}


void lis3dh_init_fifo_version(void)
{
	config_ctrl_reg1(ODR_1344HZ, ENABLE_NORMAL_POWER_MODE, ENABLE_ALL_AXES, 1);
	HAL_Delay(10);
	config_ctrl_reg2(HPM_NORMAL_MODE_After_RESET, HPCF_CUTOFF_2,
				DISABLE_FILTERED_DATA_SELECTION, DISABLE_HPF_CLICK,
				DISABLE_HPF_AOI_INT2, DISABLE_HPF_AOI_INT1, 1);
	HAL_Delay(10);
	// Enable INT1 FIFO watermark so INT pin asserts when FIFO >= watermark
	config_ctrl_reg3(DISABLE_INT1_CLICK, DISABLE_INT1_AOI1, DISABLE_INT1_AOI2,
				DISABLE_INT1_DATA_READY1, DISABLE_INT1_DATA_READY2,
				ENABLE_INT1_FIFO_WATERMARK, DISABLE_INT1_OVERRUN, 1);
	HAL_Delay(10);
	config_ctrl_reg4(ENABLE_BLOCK_DATA_UPDATE, ENABLE_LITTLE_ENDIAN, FS_2G,
				DISABLE_HIGH_RESOLUTION_MODE, DISABLE_SELF_TEST, ENABLE_3_WIRE_SPI, 1);
	HAL_Delay(10);
	// Enable FIFO in CTRL_REG5
	config_ctrl_reg5(NORMAL_OPERATION, ENABLE_FIFO,
				DISABLE_LATCHED_INT1, DISABLE_4D_INT1, 1);
	HAL_Delay(10);
	// Set it to FIFO mode and set watermark to 10 samples
	config_fifo_ctrl_reg(FIFO_MODE, ENABLE_TRIGGER_INT1, FIFO_WATERMARK_LEVEL(31), 1);
}



void check_fifo_ready(void)
{
	uint8_t fifo_status = read_fifo_src_reg(1);
	if ((fifo_status & EXCEEDED_WTM) || (fifo_status & FIFO_OVERRUN))
	{
		HAL_UART_Transmit(&huart3, (uint8_t *)"FIFO ready\r\n", strlen("FIFO ready\r\n"), HAL_MAX_DELAY);
		uint8_t samples_in_fifo = fifo_status & 0x1F; // Bits 0-4 indicate number of samples
		char msg[64];
		snprintf(msg, sizeof(msg), "Samples in FIFO: %d\r\n", samples_in_fifo);
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		int16_t x, y, z;
		for (uint8_t i = 0; i < samples_in_fifo; i++)
		{
			read_XYZ_Acceleration(&x, &y, &z);
			print_XYZ_Acceleration(x, y, z);
		}
		config_fifo_ctrl_reg(BYPASS_MODE, ENABLE_TRIGGER_INT1, FIFO_WATERMARK_LEVEL(31), 1);



	}
	else
	{
		HAL_UART_Transmit(&huart3, (uint8_t *)"FIFO not ready\r\n", strlen("FIFO not ready\r\n"), HAL_MAX_DELAY);
	}

}



void read_XYZ_Acceleration(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t buffer[6];
	//	lis3dh_read_registers(OUT_X_L | 0x80, buffer, 6); // Auto-increment address with MSB set
	lis3dh_i2c_read(OUT_X_L | 0x80, buffer, 6); // Auto-increment address with MSB set

	// Combine low and high bytes for each axis
	*x = (int16_t)(buffer[1] << 8 | buffer[0]);
	*y = (int16_t)(buffer[3] << 8 | buffer[2]);
	*z = (int16_t)(buffer[5] << 8 | buffer[4]);
}

// Helper function to convert raw LIS3DH data to mg (integer math)
static int32_t lis3dh_raw_to_mg_int(int16_t raw, uint8_t ctrl_reg_1, uint8_t ctrl_reg_4)
{
    int32_t sensitivity_x10 = 0; // sensitivity * 10, for integer math
    // Full-scale selection
    // Way the sensitivity is calculated is based on the full scale range.
    // For instance, when +/-2g is selected there is range of 4000mg=4g,
    // 8000mg for +/-4g, 16000mg for +/-8g, and 32000mg for +/-16g.
    // The step is determined by the resolution mode:
    // 2^8=256 steps for 8-bit (low-power mode)
    // 2^10=1024 steps for 10-bit (normal mode)
    // 2^12=4096 steps for 12-bit (high-resolution mode)

    // Calculate sensitivity in mg/LSB * 10 for integer math, 10 is the scale factor for decimal precision
    // sensitivity_x10 = (full_scale_range_mg) / (number_of_steps) * 10

    // Resolution selection
    if ((ctrl_reg_1 & 0x08) == ENABLE_LOW_POWER_MODE)
    {
        raw = raw >> 8; // 8-bit
        switch (ctrl_reg_4 & 0x30)
        {
            case FS_2G:
            	sensitivity_x10 = 156;
            	break;
            case FS_4G:
            	sensitivity_x10 = 312;
            	break;
            case FS_8G:
            	sensitivity_x10 = 625;
            	break;
            case FS_16G:
            	sensitivity_x10 = 1250;
            	break;
        }
    }
    else if ((ctrl_reg_4 & 0x08) == ENABLE_HIGH_RESOLUTION_MODE)
    {
        raw = raw >> 4; // 12-bit
        switch (ctrl_reg_4 & 0x30)
        {
            case FS_2G:
            	sensitivity_x10 = 10;
            	break; // 0.98 mg/LSB rounded to 1.0 mg/LSB
            case FS_4G:
            	sensitivity_x10 = 20;
            	break;
            case FS_8G:
            	sensitivity_x10 = 39;
            	break;
            case FS_16G:
            	sensitivity_x10 = 78;
            	break;
        }
    }
    else
    {
        raw = raw >> 6; // 10-bit
        switch (ctrl_reg_4 & 0x30)
        {
            case FS_2G:
                sensitivity_x10 = 39; // 3.9 mg/LSB * 10
                break;
            case FS_4G:
                sensitivity_x10 = 78;
                break;
            case FS_8G:
                sensitivity_x10 = 156;
                break;
            case FS_16G:
                sensitivity_x10 = 312;
                break;
            default:
                sensitivity_x10 = 39;
                break;
        }
    }
    // Return mg value (integer)
    return (int32_t)raw * sensitivity_x10 / 10;
}

void print_XYZ_Acceleration(int16_t x, int16_t y, int16_t z)
{
    char msg[128];
    uint8_t ctrl_reg_1_val = 0;
    uint8_t ctrl_reg_4_val = 0;
    // Read CTRL_REG
    ctrl_reg_1_val = read_ctrl_reg1();
    ctrl_reg_4_val = read_ctrl_reg4();
    // Convert raw to mg (integer)
    int32_t x_mg = lis3dh_raw_to_mg_int(x, ctrl_reg_1_val, ctrl_reg_4_val);
    int32_t y_mg = lis3dh_raw_to_mg_int(y, ctrl_reg_1_val, ctrl_reg_4_val);
    int32_t z_mg = lis3dh_raw_to_mg_int(z, ctrl_reg_1_val, ctrl_reg_4_val);
    // Print mg values (integer)
    snprintf(msg, sizeof(msg), "X=%ld mg Y=%ld mg Z=%ld mg\r\n", x_mg, y_mg, z_mg);
    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void read_XY_Acceleration(int16_t *x, int16_t *y)
{
	uint8_t buffer[4];
	//	lis3dh_read_registers(OUT_X_L | 0x80, buffer, 4); // Auto-increment address with MSB set
	lis3dh_i2c_read(OUT_X_L | 0x80, buffer, 4); // Auto-increment address with MSB set

	// Combine low and high bytes for each axis
	*x = (int16_t)(buffer[1] << 8 | buffer[0]);
	*y = (int16_t)(buffer[3] << 8 | buffer[2]);
}

void print_XY_Acceleration(int16_t x, int16_t y)
{
	char msg[128];
	uint8_t ctrl_reg_1_val = 0;
	uint8_t ctrl_reg_4_val = 0;
	// Read CTRL_REG
	ctrl_reg_1_val = read_ctrl_reg1();
	ctrl_reg_4_val = read_ctrl_reg4();
	// Check which mode it is in by check ctrl reg 1 on power mode pin
	if ((ctrl_reg_1_val & 0x08) == ENABLE_LOW_POWER_MODE)
	{
		// Low power mode: right shift by 8 to get 8-bit data
		x = x >> 8;
		y = y >> 8;
	}
	else
	{
		// Normal or high-resolution mode
		if ((ctrl_reg_4_val & 0x08) == ENABLE_HIGH_RESOLUTION_MODE)
		{
			// High-resolution mode: right shift by 4 to get 12-bit data
			x = x >> 4;
			y = y >> 4;
		}
		else
		{
			// Normal mode: right shift by 6 to get 10-bit data
			x = x >> 6;
			y = y >> 6;
		}
	}
	snprintf(msg, sizeof(msg), "X=%d Y=%d\r\n", x, y);
	HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void read_XZ_Acceleration(int16_t *x, int16_t *z)
{
	uint8_t buffer[4];
	//	lis3dh_read_registers(OUT_X_L | 0x80, buffer, 4); // Auto-increment address with MSB set
	lis3dh_i2c_read(OUT_X_L | 0x80, buffer, 4); // Auto-increment address with MSB set

	// Combine low and high bytes for each axis
	*x = (int16_t)(buffer[1] << 8 | buffer[0]);
	*z = (int16_t)(buffer[3] << 8 | buffer[2]);
}

void print_XZ_Acceleration(int16_t x, int16_t z)
{
	char msg[128];
	uint8_t ctrl_reg_1_val = 0;
	uint8_t ctrl_reg_4_val = 0;
	// Read CTRL_REG
	ctrl_reg_1_val = read_ctrl_reg1();
	ctrl_reg_4_val = read_ctrl_reg4();
	// Check which mode it is in by check ctrl reg 1 on power mode pin
	if ((ctrl_reg_1_val & 0x08) == ENABLE_LOW_POWER_MODE)
	{
		// Low power mode: right shift by 8 to get 8-bit data
		x = x >> 8;
		z = z >> 8;
	}
	else
	{
		// Normal or high-resolution mode
		if ((ctrl_reg_4_val & 0x08) == ENABLE_HIGH_RESOLUTION_MODE)
		{
			// High-resolution mode: right shift by 4 to get 12-bit data
			x = x >> 4;
			z = z >> 4;
		}
		else
		{
			// Normal mode: right shift by 6 to get 10-bit data
			x = x >> 6;
			z = z >> 6;
		}
	}
	snprintf(msg, sizeof(msg), "X=%d Z=%d\r\n", x, z);
	HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void read_YZ_Acceleration(int16_t *y, int16_t *z)
{
	uint8_t buffer[4];
	//	lis3dh_read_registers(OUT_Y_L | 0x80, buffer, 4); // Auto-increment address with MSB set
	lis3dh_i2c_read(OUT_Y_L | 0x80, buffer, 4); // Auto-increment address with MSB set

	// Combine low and high bytes for each axis
	*y = (int16_t)(buffer[1] << 8 | buffer[0]);
	*z = (int16_t)(buffer[3] << 8 | buffer[2]);
}

void print_YZ_Acceleration(int16_t y, int16_t z)
{
	char msg[128];
	uint8_t ctrl_reg_1_val = 0;
	uint8_t ctrl_reg_4_val = 0;
	// Read CTRL_REG
	ctrl_reg_1_val = read_ctrl_reg1();
	ctrl_reg_4_val = read_ctrl_reg4();
	// Check which mode it is in by check ctrl reg 1 on power mode pin
	if ((ctrl_reg_1_val & 0x08) == ENABLE_LOW_POWER_MODE)
	{
		// Low power mode: right shift by 8 to get 8-bit data
		y = y >> 8;
		z = z >> 8;
	}
	else
	{
		// Normal or high-resolution mode
		if ((ctrl_reg_4_val & 0x08) == ENABLE_HIGH_RESOLUTION_MODE)
		{
			// High-resolution mode: right shift by 4 to get 12-bit data
			y = y >> 4;
			z = z >> 4;
		}
		else
		{
			// Normal mode: right shift by 6 to get 10-bit data
			y = y >> 6;
			z = z >> 6;
		}
	}
	snprintf(msg, sizeof(msg), "Y=%d Z=%d\r\n", y, z);
	HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void read_X_Acceleration(int16_t *x)
{
	uint8_t buffer[2];
	//	lis3dh_read_registers(OUT_X_L | 0x80, buffer, 2); // Auto-increment address with MSB set
	lis3dh_i2c_read(OUT_X_L | 0x80, buffer, 2); // Auto-increment address with MSB set

	// Combine low and high bytes for X axis
	*x = (int16_t)(buffer[1] << 8 | buffer[0]);
}


void print_X_Acceleration(int16_t x)
{
	char msg[128];
	uint8_t ctrl_reg_1_val = 0;
	uint8_t ctrl_reg_4_val = 0;
	// Read CTRL_REG
	ctrl_reg_1_val = read_ctrl_reg1();
	ctrl_reg_4_val = read_ctrl_reg4();
	// Check which mode it is in by check ctrl reg 1 on power mode pin
	if ((ctrl_reg_1_val & 0x08) == ENABLE_LOW_POWER_MODE)
	{
		// Low power mode: right shift by 8 to get 8-bit data
		x = x >> 8;
	}
	else
	{
		// Normal or high-resolution mode
		if ((ctrl_reg_4_val & 0x08) == ENABLE_HIGH_RESOLUTION_MODE)
		{
			// High-resolution mode: right shift by 4 to get 12-bit data
			x = x >> 4;
		}
		else
		{
			// Normal mode: right shift by 6 to get 10-bit data
			x = x >> 6;
		}
	}
	snprintf(msg, sizeof(msg), "X=%d\r\n", x);
	HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void read_Y_Acceleration(int16_t *y)
{
	uint8_t buffer[2];
	//	lis3dh_read_registers(OUT_Y_L | 0x80, buffer, 2); // Auto-increment address with MSB set
	lis3dh_i2c_read(OUT_Y_L | 0x80, buffer, 2); // Auto-increment address with MSB set

	// Combine low and high bytes for Y axis
	*y = (int16_t)(buffer[1] << 8 | buffer[0]);
}


void print_Y_Acceleration(int16_t y)
{
	char msg[128];
	uint8_t ctrl_reg_1_val = 0;
	uint8_t ctrl_reg_4_val = 0;
	// Read CTRL_REG
	ctrl_reg_1_val = read_ctrl_reg1();
	ctrl_reg_4_val = read_ctrl_reg4();
	// Check which mode it is in by check ctrl reg 1 on power mode pin
	if ((ctrl_reg_1_val & 0x08) == ENABLE_LOW_POWER_MODE)
	{
		// Low power mode: right shift by 8 to get 8-bit data
		y = y >> 8;
	}
	else
	{
		// Normal or high-resolution mode
		if ((ctrl_reg_4_val & 0x08) == ENABLE_HIGH_RESOLUTION_MODE)
		{
			// High-resolution mode: right shift by 4 to get 12-bit data
			y = y >> 4;
		}
		else
		{
			// Normal mode: right shift by 6 to get 10-bit data
			y = y >> 6;
		}
	}
	snprintf(msg, sizeof(msg), "Y=%d\r\n", y);
	HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void read_Z_Acceleration(int16_t *z)
{
	uint8_t buffer[2];
	//	lis3dh_read_registers(OUT_Z_L | 0x80, buffer, 2); // Auto-increment address with MSB set
	lis3dh_i2c_read(OUT_Z_L | 0x80, buffer, 2); // Auto-increment address with MSB set

	// Combine low and high bytes for Z axis
	*z = (int16_t)(buffer[1] << 8 | buffer[0]);
}

void print_Z_Acceleration(int16_t z)
{
	char msg[128];
	uint8_t ctrl_reg_1_val = 0;
	uint8_t ctrl_reg_4_val = 0;
	// Read CTRL_REG
	ctrl_reg_1_val = read_ctrl_reg1();
	ctrl_reg_4_val = read_ctrl_reg4();
	// Check which mode it is in by check ctrl reg 1 on power mode pin
	if ((ctrl_reg_1_val & 0x08) == ENABLE_LOW_POWER_MODE)
	{
		// Low power mode: right shift by 8 to get 8-bit data
		z = z >> 8;
	}
	else
	{
		// Normal or high-resolution mode
		if ((ctrl_reg_4_val & 0x08) == ENABLE_HIGH_RESOLUTION_MODE)
		{
			// High-resolution mode: right shift by 4 to get 12-bit data
			z = z >> 4;
		}
		else
		{
			// Normal mode: right shift by 6 to get 10-bit data
			z = z >> 6;
		}
	}
	snprintf(msg, sizeof(msg), "Z=%d\r\n", z);
	HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void check_all_Registers(void)
{
    // Create a small table mapping register addresses to human-readable names.
    // This lets us print both the name and the numeric address/value for easier debugging.
    typedef struct {
        uint8_t addr;
        const char *name;
    } reg_info_t;

    static const reg_info_t lis3dh_registers[] = {
        { STATUS_REG_AUX, "STATUS_REG_AUX" },
        { OUT_ADC1_L,     "OUT_ADC1_L" },
        { OUT_ADC1_H,     "OUT_ADC1_H" },
        { OUT_ADC2_L,     "OUT_ADC2_L" },
        { OUT_ADC2_H,     "OUT_ADC2_H" },
        { OUT_ADC3_L,     "OUT_ADC3_L" },
        { OUT_ADC3_H,     "OUT_ADC3_H" },
        { INT_COUNTER_REG,"INT_COUNTER_REG" },
        { WHO_AM_I,       "WHO_AM_I" },
        { TEMP_CFG_REG,   "TEMP_CFG_REG" },
        { CTRL_REG1,      "CTRL_REG1" },
        { CTRL_REG2,      "CTRL_REG2" },
        { CTRL_REG3,      "CTRL_REG3" },
        { CTRL_REG4,      "CTRL_REG4" },
        { CTRL_REG5,      "CTRL_REG5" },
        { CTRL_REG6,      "CTRL_REG6" },
        { REFERENCE,      "REFERENCE" },
        { STATUS_REG,     "STATUS_REG" },
        { OUT_X_L,        "OUT_X_L" },
        { OUT_X_H,        "OUT_X_H" },
        { OUT_Y_L,        "OUT_Y_L" },
        { OUT_Y_H,        "OUT_Y_H" },
        { OUT_Z_L,        "OUT_Z_L" },
        { OUT_Z_H,        "OUT_Z_H" },
        { FIFO_CTRL_REG,  "FIFO_CTRL_REG" },
        { FIFO_SRC_REG,   "FIFO_SRC_REG" },
        { INT1_CFG,       "INT1_CFG" },
        { INT1_SOURCE,    "INT1_SOURCE" },
        { INT1_THS,       "INT1_THS" },
        { INT1_DURATION,  "INT1_DURATION" },
        { INT2_CFG,       "INT2_CFG" },
        { INT2_SOURCE,    "INT2_SOURCE" },
        { INT2_THS,       "INT2_THS" },
        { INT2_DURATION,  "INT2_DURATION" },
        { CLICK_CFG,      "CLICK_CFG" },
        { CLICK_SRC,      "CLICK_SRC" },
        { CLICK_THS,      "CLICK_THS" },
        { TIME_LIMIT,     "TIME_LIMIT" },
        { TIME_LATENCY,   "TIME_LATENCY" },
        { TIME_WINDOW,    "TIME_WINDOW" },
        { ACT_THS,        "ACT_THS" },
        { ACT_DUR,        "ACT_DUR" }
    };

    const size_t reg_count = sizeof(lis3dh_registers) / sizeof(lis3dh_registers[0]);

    uint8_t value;
    char msg[80];
    HAL_UART_Transmit(&huart3, (uint8_t *)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);
    snprintf(msg, sizeof(msg), "----- Register Values -----\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    for (size_t i = 0; i < reg_count; ++i)
    {
//        lis3dh_read_registers(lis3dh_registers[i].addr, &value, 1);
    	lis3dh_i2c_read(lis3dh_registers[i].addr, &value, 1);
        // Print: <NAME> (0xAA): 0xVV
        snprintf(msg, sizeof(msg), "%s (0x%02X): 0x%02X\r\n", lis3dh_registers[i].name, lis3dh_registers[i].addr, value);
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }
    HAL_UART_Transmit(&huart3, (uint8_t *)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);

}



// Read raw temperature value from LIS3DH (ADC3)
int16_t lis3dh_read_temperature_raw(void)
{
    uint8_t buffer[2];
    lis3dh_i2c_read(OUT_ADC3_L | 0x80, buffer, 2);
    return (int16_t)(buffer[1] << 8 | buffer[0]);
}

// Print temperature value (raw and relative to 25°C)
void lis3dh_print_temperature(void) {
    int16_t temp_raw = lis3dh_read_temperature_raw();
    int16_t temp_c_x10 = 250 + ((temp_raw * 10) / 256); // tenths of a degree
    char msg[64];
    snprintf(msg, sizeof(msg), "Temperature: raw=%d, approx=%d.%d C\r\n", temp_raw, temp_c_x10 / 10, temp_c_x10 % 10);
    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}
