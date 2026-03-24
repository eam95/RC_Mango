/*
 * LIS3DH_Config.c
 *
 *  Created on: Oct 20, 2025
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


// Read WHO_AM_I and print over UART. Call with no arguments; LIS3DH_ADDRESS is used.
void who_am_i(void)
{
    uint8_t id = 0;
//    lis3dh_read_registers(WHO_AM_I, &id, 1);
    lis3dh_i2c_read(WHO_AM_I, &id, 1);
    char msg[64];
    snprintf(msg, sizeof(msg), "LIS3DH WHO_AM_I: 0x%02X\r\n", id);
    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}



uint8_t read_int_counter_reg(uint8_t print)
 {
     uint8_t regval = 0;
//	lis3dh_read_registers(INT_COUNTER_REG, &regval, 1);
    lis3dh_i2c_read(INT_COUNTER_REG, &regval, 1);

     if (print == 1)
     {
         char buf[64];
         // Divider/header
         snprintf(buf, sizeof(buf), "----- INT_COUNTER_REG -----\r\n");
         HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

         // Line 1: raw value
         snprintf(buf, sizeof(buf), "INT_COUNTER_REG (read) = 0x%02X\r\n", regval);
         HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

         // Line 2: Interrupt count value
         snprintf(buf, sizeof(buf), "Interrupt counter value: %d\r\n", regval);
         HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

         // Footer blank line for readability
         snprintf(buf, sizeof(buf), "\r\n");
         HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
     }
     return regval;
}



void config_temp_cfg_reg(uint8_t TempEnable, uint8_t print)
{
	uint8_t config = 0;
	config = TempEnable;
//	lis3dh_write_registers(TEMP_CFG_REG, &config, 1);
	lis3dh_i2c_write(TEMP_CFG_REG, &config, 1);
	if (print == 1)
	{
		print_config_temp_cfg_reg();
	}
}

uint8_t read_temp_cfg_reg(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(TEMP_CFG_REG, &regval, 1);
	return regval;
}

void print_config_temp_cfg_reg(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(TEMP_CFG_REG, &regval, 1);
	lis3dh_i2c_read(TEMP_CFG_REG, &regval, 1);

	char buf[64];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- TEMP_CFG_REG -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 1: raw value
	snprintf(buf, sizeof(buf), "TEMP_CFG_REG (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 2: Temperature sensor enable
	if (regval & ENABLE_TEMPERATURE_SENSOR)
		snprintf(buf, sizeof(buf), "Temperature sensor: enabled\r\n");
	else
		snprintf(buf, sizeof(buf), "Temperature sensor: disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line for readability
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_ctrl_reg1(uint8_t ODR, uint8_t PowerMode, uint8_t SetAxis, uint8_t print)
{
    uint8_t config = 0;
    config = ODR | PowerMode | SetAxis;
//    lis3dh_write_registers(CTRL_REG1, &config, 1);
    lis3dh_i2c_write(CTRL_REG1, &config, 1);
    HAL_Delay(1);
    if (print == 1)
    {
    	print_config_ctrl_reg1();
    }
}
uint8_t read_ctrl_reg1(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(CTRL_REG1, &regval, 1);
	return regval;
}


void print_config_ctrl_reg1(void)
{
    uint8_t regval = 0;
//    lis3dh_read_registers(CTRL_REG1, &regval, 1);
    lis3dh_i2c_read(CTRL_REG1, &regval, 1);

    char buf[64];
    // Divider/header
    snprintf(buf, sizeof(buf), "----- CTRL_REG1 -----\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Line 1: raw value
    snprintf(buf, sizeof(buf), "CTRL_REG1 (read) = 0x%02X\r\n", regval);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Line 2: ODR
    switch (regval & 0xF0)
    {
        case ODR_POWER_DOWN:       snprintf(buf, sizeof(buf), "ODR: Power-down\r\n"); break;
        case ODR_1HZ:              snprintf(buf, sizeof(buf), "ODR: 1 Hz\r\n"); break;
        case ODR_10HZ:             snprintf(buf, sizeof(buf), "ODR: 10 Hz\r\n"); break;
        case ODR_25HZ:             snprintf(buf, sizeof(buf), "ODR: 25 Hz\r\n"); break;
        case ODR_50HZ:             snprintf(buf, sizeof(buf), "ODR: 50 Hz\r\n"); break;
        case ODR_100HZ:            snprintf(buf, sizeof(buf), "ODR: 100 Hz\r\n"); break;
        case ODR_200HZ:            snprintf(buf, sizeof(buf), "ODR: 200 Hz\r\n"); break;
        case ODR_400HZ:            snprintf(buf, sizeof(buf), "ODR: 400 Hz\r\n"); break;
        case ODR_1344HZ:           snprintf(buf, sizeof(buf), "ODR: 1344 Hz\r\n"); break;
        case ODR_1620HZ_LP:        snprintf(buf, sizeof(buf), "ODR: 1620 Hz (LP)\r\n"); break;
        case ODR_5376HZ_LP:        snprintf(buf, sizeof(buf), "ODR: 5376 Hz (LP)\r\n"); break;
        default:                   snprintf(buf, sizeof(buf), "ODR: Unknown(0x%02X)\r\n", regval & 0xF0); break;
    }
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Line 3: Power mode
    if (regval & ENABLE_LOW_POWER_MODE)
        snprintf(buf, sizeof(buf), "Mode: Low-power mode\r\n");
    else
        snprintf(buf, sizeof(buf), "Mode: Normal-power mode\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Line 4: Axes
    uint8_t x = (regval & ENABLE_X_AXIS) != 0;
    uint8_t y = (regval & ENABLE_Y_AXIS) != 0;
    uint8_t z = (regval & ENABLE_Z_AXIS) != 0;
    if (x && y && z) snprintf(buf, sizeof(buf), "Axes: X, Y and Z enabled\r\n");
    else if (x && y)  snprintf(buf, sizeof(buf), "Axes: X and Y enabled\r\n");
    else if (x && z)  snprintf(buf, sizeof(buf), "Axes: X and Z enabled\r\n");
    else if (y && z)  snprintf(buf, sizeof(buf), "Axes: Y and Z enabled\r\n");
    else if (x)        snprintf(buf, sizeof(buf), "Axes: X enabled only\r\n");
    else if (y)        snprintf(buf, sizeof(buf), "Axes: Y enabled only\r\n");
    else if (z)        snprintf(buf, sizeof(buf), "Axes: Z enabled only\r\n");
    else               snprintf(buf, sizeof(buf), "Axes: No axis enabled\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Footer blank line for readability
    snprintf(buf, sizeof(buf), "\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_ctrl_reg2(uint8_t HighPassMode, uint8_t CutoffFreq, uint8_t FilteredData, uint8_t HPF_Click,
						 uint8_t HPF_AOI2, uint8_t HPF_AOI1, uint8_t print)
{
	uint8_t config = 0;
	config = HighPassMode | CutoffFreq | FilteredData | HPF_Click | HPF_AOI2 | HPF_AOI1;
//	lis3dh_write_registers(CTRL_REG2, &config, 1);
	lis3dh_i2c_write(CTRL_REG2, &config, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_ctrl_reg2();
	}

}

uint8_t read_ctrl_reg2(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(CTRL_REG2, &regval, 1);
	return regval;
}


void print_config_ctrl_reg2(void)
{
    uint8_t regval = 0;
//    lis3dh_read_registers(CTRL_REG2, &regval, 1);
    lis3dh_i2c_read(CTRL_REG2, &regval, 1);

    char buf[64];
    // Divider/header
    snprintf(buf, sizeof(buf), "----- CTRL_REG2 -----\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    snprintf(buf, sizeof(buf), "CTRL_REG2 (read) = 0x%02X\r\n", regval);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // HPM
    switch (regval & 0xC0)
    {
        case HPM_NORMAL_MODE_After_RESET: snprintf(buf, sizeof(buf), "HPM: Normal mode after reset\r\n"); break;
        case HPM_REF_SIGNAL:              snprintf(buf, sizeof(buf), "HPM: Reference signal\r\n"); break;
        case HPM_NORMAL_MODE:             snprintf(buf, sizeof(buf), "HPM: Normal mode\r\n"); break;
        case HPM_AUTORESET:               snprintf(buf, sizeof(buf), "HPM: Auto-reset\r\n"); break;
        default:                          snprintf(buf, sizeof(buf), "HPM: Unknown(0x%02X)\r\n", regval & 0xC0); break;
    }
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Filtered data selection
    if (regval & ENABLE_FILTERED_DATA_SELECTION) snprintf(buf, sizeof(buf), "Filtered data selection: enabled\r\n");
    else snprintf(buf, sizeof(buf), "Filtered data selection: disabled\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // HPF click
    if (regval & ENABLE_HPF_CLICK) snprintf(buf, sizeof(buf), "HPF for click: enabled\r\n");
    else snprintf(buf, sizeof(buf), "HPF for click: disabled\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // HPIS2
    if (regval & ENABLE_HPF_AOI_INT2) snprintf(buf, sizeof(buf), "HPF AOI2: enabled\r\n");
    else snprintf(buf, sizeof(buf), "HPF AOI2: disabled\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // HPIS1
    if (regval & ENABLE_HPF_AOI_INT1) snprintf(buf, sizeof(buf), "HPF AOI1: enabled\r\n");
    else snprintf(buf, sizeof(buf), "HPF AOI1: disabled\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Footer blank line
    snprintf(buf, sizeof(buf), "\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_ctrl_reg3(uint8_t Int1_Click, uint8_t Int1_AOI1, uint8_t Int1_AOI2,
							uint8_t Int1_DataReady1, uint8_t Int1_DataReady2,
							uint8_t Int1_FifoWatermark, uint8_t Int1_Overrun,
							uint8_t print)
{
	uint8_t config = 0;
	config = Int1_Click | Int1_AOI1 | Int1_AOI2 | Int1_DataReady1 |
			 Int1_DataReady2 | Int1_FifoWatermark | Int1_Overrun;
//	lis3dh_write_registers(CTRL_REG3, &config, 1);
	lis3dh_i2c_write(CTRL_REG3, &config, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_ctrl_reg3();
	}

}

uint8_t read_ctrl_reg3(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(CTRL_REG3, &regval, 1);
	return regval;
}


void print_config_ctrl_reg3(void)
{
    uint8_t regval = 0;
//    lis3dh_read_registers(CTRL_REG3, &regval, 1);
    lis3dh_i2c_read(CTRL_REG3, &regval, 1);

    char buf[128];
    // Divider/header
    snprintf(buf, sizeof(buf), "----- CTRL_REG3 -----\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    snprintf(buf, sizeof(buf), "CTRL_REG3 (read) = 0x%02X\r\n", regval);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    if (regval & ENABLE_INT1_CLICK) {
        snprintf(buf, sizeof(buf), "INT1: Click enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }
    if (regval & ENABLE_INT1_AOI1) {
        snprintf(buf, sizeof(buf), "INT1: AOI1 enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }
    if (regval & ENABLE_INT1_AOI2) {
        snprintf(buf, sizeof(buf), "INT1: AOI2 enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }
    if (regval & ENABLE_INT1_DATA_READY1) {
        snprintf(buf, sizeof(buf), "INT1: DataReady1 enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }
    if (regval & ENABLE_INT1_DATA_READY2) {
        snprintf(buf, sizeof(buf), "INT1: DataReady2 enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }
    if (regval & ENABLE_INT1_FIFO_WATERMARK) {
        snprintf(buf, sizeof(buf), "INT1: FIFO watermark enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }
    if (regval & ENABLE_INT1_OVERRUN) {
        snprintf(buf, sizeof(buf), "INT1: Overrun interrupt enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }
    if (regval & 0x01) {
        snprintf(buf, sizeof(buf), "INT1: FIFO empty interrupt enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }

    // If none of the bits printed, print a line saying none enabled
    if ((regval & (ENABLE_INT1_CLICK | ENABLE_INT1_AOI1 | ENABLE_INT1_AOI2 |
                   ENABLE_INT1_DATA_READY1 | ENABLE_INT1_DATA_READY2 |
                   ENABLE_INT1_FIFO_WATERMARK | ENABLE_INT1_OVERRUN | 0x01)) == 0)
    {
        snprintf(buf, sizeof(buf), "INT1: No features enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    }

    // Footer blank line
    snprintf(buf, sizeof(buf), "\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_ctrl_reg4(uint8_t BlockDataUpdate, uint8_t Endianness,
							uint8_t FullScale, uint8_t HighResMode,
							uint8_t SelfTest, uint8_t SPIMode,
							uint8_t print)
{
	uint8_t config = 0;
	config = BlockDataUpdate | Endianness | FullScale | HighResMode | SelfTest | SPIMode;
//	lis3dh_write_registers(CTRL_REG4, &config, 1);
	lis3dh_i2c_write(CTRL_REG4, &config, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_ctrl_reg4();
	}

}

uint8_t read_ctrl_reg4(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(CTRL_REG4, &regval, 1);
	return regval;
}


void print_config_ctrl_reg4(void)
{
    uint8_t regval = 0;
//    lis3dh_read_registers(CTRL_REG4, &regval, 1);
    lis3dh_i2c_read(CTRL_REG4, &regval, 1);

    char buf[128];
    // Divider/header
    snprintf(buf, sizeof(buf), "----- CTRL_REG4 -----\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    snprintf(buf, sizeof(buf), "CTRL_REG4 (read) = 0x%02X\r\n", regval);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // BDU
    if (regval & ENABLE_BLOCK_DATA_UPDATE) snprintf(buf, sizeof(buf), "BDU: enabled\r\n");
    else snprintf(buf, sizeof(buf), "BDU: disabled\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Endianness
    if (regval & ENABLE_BIG_ENDIAN) snprintf(buf, sizeof(buf), "Endian: big\r\n");
    else snprintf(buf, sizeof(buf), "Endian: little\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Full scale
    switch (regval & 0x30)
    {
        case FS_2G:  snprintf(buf, sizeof(buf), "FS: +-2g\r\n"); break;
        case FS_4G:  snprintf(buf, sizeof(buf), "FS: +-4g\r\n"); break;
        case FS_8G:  snprintf(buf, sizeof(buf), "FS: +-8g\r\n"); break;
        case FS_16G: snprintf(buf, sizeof(buf), "FS: +-16g\r\n"); break;
        default:     snprintf(buf, sizeof(buf), "FS: Unknown\r\n"); break;
    }
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // High-res
    if (regval & ENABLE_HIGH_RESOLUTION_MODE) snprintf(buf, sizeof(buf), "High-res: enabled\r\n");
    else snprintf(buf, sizeof(buf), "High-res: disabled\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Self-test bits
    uint8_t st = regval & 0x06;
    if (st == 0) {
        snprintf(buf, sizeof(buf), "Self-test: disabled\r\n");
        HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    } else {
        if (st & ENABLE_SELF_TEST1) { snprintf(buf, sizeof(buf), "Self-test: ST1 set\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY); }
        if (st & ENABLE_SELF_TEST0) { snprintf(buf, sizeof(buf), "Self-test: ST0 set\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY); }
    }

    // SPI mode
    if (regval & 0x01) snprintf(buf, sizeof(buf), "SPI: 3-wire\r\n");
    else snprintf(buf, sizeof(buf), "SPI: 4-wire\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

    // Footer blank line
    snprintf(buf, sizeof(buf), "\r\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_ctrl_reg5(uint8_t Boot, uint8_t SetFIFO, uint8_t LatchInterrupt,
					uint8_t set_4D, uint8_t print)
{
	uint8_t config = 0;
	config = Boot | SetFIFO | LatchInterrupt | set_4D;
	// Fixed: write to CTRL_REG5 (was incorrectly writing to CTRL_REG4)
//	lis3dh_write_registers(CTRL_REG5, &config, 1);
	lis3dh_i2c_write(CTRL_REG5, &config, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_ctrl_reg5();
	}
}

uint8_t read_ctrl_reg5(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(CTRL_REG5, &regval, 1);
	return regval;
}


void print_config_ctrl_reg5(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(CTRL_REG5, &regval, 1);
	lis3dh_i2c_read(CTRL_REG5, &regval, 1);

	char buf[128];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- CTRL_REG5 -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "CTRL_REG5 (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Boot
	if (regval & REBOOT_MEMORY_CONTENT) snprintf(buf, sizeof(buf), "Boot: enabled\r\n");
	else snprintf(buf, sizeof(buf), "Boot: disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// FIFO
	if (regval & ENABLE_FIFO) snprintf(buf, sizeof(buf), "FIFO: enabled\r\n");
	else snprintf(buf, sizeof(buf), "FIFO: disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Latch Interrupt
	if (regval & ENABLE_LATCHED_INT1) snprintf(buf, sizeof(buf), "Latch Interrupt: enabled\r\n");
	else snprintf(buf, sizeof(buf), "Latch Interrupt: disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// 4D detection
	if (regval & ENABLE_4D_INT1) snprintf(buf, sizeof(buf), "4D Detection: enabled\r\n");
	else snprintf(buf, sizeof(buf), "4D Detection: disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_ctrl_reg6(uint8_t Int2_Click, uint8_t Int2_AOI1, uint8_t Int2_AOI2,
							uint8_t BootInt2, uint8_t Int2_Activity,
							uint8_t Int_Polarity, uint8_t print)
{
	uint8_t config = 0;
	config = Int2_Click | Int2_AOI1 | Int2_AOI2 | BootInt2 |
			 Int2_Activity | Int_Polarity;
//	lis3dh_write_registers(CTRL_REG6, &config, 1);
	lis3dh_i2c_write(CTRL_REG6, &config, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_ctrl_reg6();
	}

}

uint8_t read_ctrl_reg6(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(CTRL_REG6, &regval, 1);
	return regval;
}


void print_config_ctrl_reg6(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(CTRL_REG6, &regval, 1);
	lis3dh_i2c_read(CTRL_REG6, &regval, 1);
     
     char buf[128];
     // Divider/header
     snprintf(buf, sizeof(buf), "----- CTRL_REG6 -----\r\n");
     HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

     snprintf(buf, sizeof(buf), "CTRL_REG6 (read) = 0x%02X\r\n", regval);
     HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

     if (regval & ENABLE_INT2_CLICK) {
		snprintf(buf, sizeof(buf), "INT2: Click enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT2_AOI1) {
		snprintf(buf, sizeof(buf), "INT2: AOI1 enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT2_AOI2) {
		snprintf(buf, sizeof(buf), "INT2: AOI2 enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_BOOT_INT2) {
		snprintf(buf, sizeof(buf), "INT2: Boot interrupt enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT2_ACTIVITY) {
		snprintf(buf, sizeof(buf), "INT2: Activity interrupt enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT_Polarity) {
		snprintf(buf, sizeof(buf), "INT pins: Active high\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	else {
		snprintf(buf, sizeof(buf), "INT pins: Active low\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		}
	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



uint8_t read_status_reg(uint8_t print)
{
	uint8_t status = 0;
//	lis3dh_read_registers(STATUS_REG, &status, 1);
	lis3dh_i2c_read(STATUS_REG, &status, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		print_status_reg(status);
	}
	return status;
}



void print_status_reg(uint8_t status)
{
	// Print individual status bits
	char msg[128];
	snprintf(msg, sizeof(msg), "----- STATUS_REG -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	snprintf(msg, sizeof(msg), "STATUS_REG (read) = 0x%02X\r\n", status);
	HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

	if (status & STATUS_REG_ZYXOR) {
		snprintf(msg, sizeof(msg), "Status: X, Y, Z axis data overrun\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	}
	if (status & STATUS_REG_ZOR) {
		snprintf(msg, sizeof(msg), "Status: Z axis data overrun\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	}
	if (status & STATUS_REG_YOR) {
		snprintf(msg, sizeof(msg), "Status: Y axis data overrun\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	}
	if (status & STATUS_REG_XOR) {
		snprintf(msg, sizeof(msg), "Status: X axis data overrun\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	}
	if (status & STATUS_REG_ZYXDA) {
		snprintf(msg, sizeof(msg), "Status: New data available for X, Y, Z axes\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	}
	if (status & STATUS_REG_ZDA) {
		snprintf(msg, sizeof(msg), "Status: New data available for Z axis\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	}
	if (status & STATUS_REG_YDA) {
		snprintf(msg, sizeof(msg), "Status: New data available for Y axis\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	}
	if (status & STATUS_REG_XDA) {
		snprintf(msg, sizeof(msg), "Status: New data available for X axis\r\n");
		HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	}
}



void config_fifo_ctrl_reg(uint8_t FifoMode, uint8_t Trigger , uint8_t WatermarkLevel, uint8_t print)
{
	uint8_t config = 0;
	config = FifoMode | Trigger | WatermarkLevel;
//	lis3dh_write_registers(FIFO_CTRL_REG, &config, 1);
	lis3dh_i2c_write(FIFO_CTRL_REG, &config, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_fifo_ctrl_reg();
	}

}

uint8_t read_fifo_ctrl_reg(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(FIFO_CTRL_REG, &regval, 1);
	return regval;
}


void print_config_fifo_ctrl_reg(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(FIFO_CTRL_REG, &regval, 1);
	lis3dh_i2c_read(FIFO_CTRL_REG, &regval, 1);

	char buf[128];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- FIFO_CTRL_REG -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "FIFO_CTRL_REG (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	// FIFO mode
	switch (regval & 0xC0)
	{
		case BYPASS_MODE:          snprintf(buf, sizeof(buf), "FIFO Mode: Bypass mode\r\n"); break;
		case FIFO_MODE:            snprintf(buf, sizeof(buf), "FIFO Mode: FIFO mode\r\n"); break;
		case STREAM_MODE:          snprintf(buf, sizeof(buf), "FIFO Mode: Stream mode\r\n"); break;
		case STREAM_TO_FIFO_MODE:  snprintf(buf, sizeof(buf), "FIFO Mode: Stream-to-FIFO mode\r\n"); break;
	}
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	// Trigger
	if (regval & ENABLE_TRIGGER_INT1) snprintf(buf, sizeof(buf), "FIFO Trigger: enabled\r\n");
	else snprintf(buf, sizeof(buf), "FIFO Trigger: disabled\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	// Watermark level
	snprintf(buf, sizeof(buf), "FIFO Watermark Level: %d\r\n", regval & 0x1F);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



uint8_t read_fifo_src_reg(uint8_t print)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(FIFO_SRC_REG, &regval, 1);
	lis3dh_i2c_read(FIFO_SRC_REG, &regval, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		print_fifo_src_reg(regval);
	}
	return regval;
}



void print_fifo_src_reg(uint8_t regval)
{
	char buf[128];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- FIFO_SRC_REG -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 1: raw value
	snprintf(buf, sizeof(buf), "FIFO_SRC_REG (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 2: Watermark status
	if (regval & EXCEEDED_WTM)
		snprintf(buf, sizeof(buf), "Watermark status: reached\r\n");
	else
		snprintf(buf, sizeof(buf), "Watermark status: not reached\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 3: Overrun status
	if (regval & FIFO_OVERRUN)
		snprintf(buf, sizeof(buf), "Overrun status: occurred\r\n");
	else
		snprintf(buf, sizeof(buf), "Overrun status: not occurred\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 4: FIFO empty status
	if (regval & FIFO_EMPTY)
		snprintf(buf, sizeof(buf), "FIFO empty status: empty\r\n");
	else
		snprintf(buf, sizeof(buf), "FIFO empty status: not empty\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Line 5: Number of samples stored in FIFO
	snprintf(buf, sizeof(buf), "Number of samples in FIFO: %d\r\n", regval & 0x1F);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line for readability
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_int1_cfg(uint8_t AOI, uint8_t set_6D, uint8_t ZHIEZ, uint8_t ZLIEZ,
						uint8_t YHIEY, uint8_t YLIEY, uint8_t XHIEZ,
						uint8_t XLIEZ, uint8_t print)
{
	uint8_t config = 0;
	config = AOI | set_6D | ZHIEZ | ZLIEZ | YHIEY | YLIEY | XHIEZ | XLIEZ;
//	lis3dh_write_registers(INT1_CFG, &config, 1);
	lis3dh_i2c_write(INT1_CFG, &config, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_int1_cfg();
	}
}

uint8_t read_int1_cfg(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(INT1_CFG, &regval, 1);
	return regval;
}


void print_config_int1_cfg(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(INT1_CFG, &regval, 1);
	lis3dh_i2c_read(INT1_CFG, &regval, 1);
	HAL_Delay(1);
	char buf[128];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- INT1_CFG -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "INT1_CFG (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Print individual configuration bits
	if (regval & INT1_AND_COMBINATION) {
		snprintf(buf, sizeof(buf), "INT1 AOI: AND combination\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	} else {
		snprintf(buf, sizeof(buf), "INT1 AOI: OR combination\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT1_6D_DETECTION) {
		snprintf(buf, sizeof(buf), "6D Detection: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT1_Z_HIGH_EVENT) {
		snprintf(buf, sizeof(buf), "Z High event: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT1_Z_LOW_EVENT) {
		snprintf(buf, sizeof(buf), "Z Low event: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT1_Y_HIGH_EVENT) {
		snprintf(buf, sizeof(buf), "Y High event: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT1_Y_LOW_EVENT) {
		snprintf(buf, sizeof(buf), "Y Low event: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT1_X_HIGH_EVENT) {
		snprintf(buf, sizeof(buf), "X High event: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_INT1_X_LOW_EVENT) {
		snprintf(buf, sizeof(buf), "X Low event: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_int1_ths(uint8_t threshold, uint8_t print)
{
//	lis3dh_write_registers(INT1_THS, &threshold, 1);
	lis3dh_i2c_write(INT1_THS, &threshold, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_int1_ths();
	}
}

uint8_t read_int1_ths(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(INT1_THS, &regval, 1);
	return regval;
}


void print_config_int1_ths(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(INT1_THS, &regval, 1);
	lis3dh_i2c_read(INT1_THS, &regval, 1);
	HAL_Delay(1);
	char buf[64];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- INT1_THS -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "INT1_THS (read) = %d\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_int1_duration(uint8_t duration, uint8_t print)
{
//	lis3dh_write_registers(INT1_DURATION, &duration, 1);
	lis3dh_i2c_write(INT1_DURATION, &duration, 1);
	HAL_Delay(1);
	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_int1_duration();
	}
}

uint8_t read_int1_duration(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(INT1_DURATION, &regval, 1);
	return regval;
}


void print_config_int1_duration(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(INT1_DURATION, &regval, 1);
	lis3dh_i2c_read(INT1_DURATION, &regval, 1);

	char buf[64];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- INT1_DURATION -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "INT1_DURATION (read) = %d\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_click_cfg(uint8_t ZS, uint8_t ZD, uint8_t YS, uint8_t YD,
						uint8_t XS, uint8_t XD, uint8_t print)
{
	uint8_t config = 0;
	config = ZS | ZD | YS | YD | XS | XD;
//	lis3dh_write_registers(CLICK_CFG, &config, 1);
	lis3dh_i2c_write(CLICK_CFG, &config, 1);

	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_click_cfg();
	}
}

uint8_t read_click_cfg(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(CLICK_CFG, &regval, 1);
	return regval;
}


void print_config_click_cfg(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(CLICK_CFG, &regval, 1);
	lis3dh_i2c_read(CLICK_CFG, &regval, 1);

	char buf[128];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- CLICK_CFG -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "CLICK_CFG (read) = 0x%02X\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Print individual configuration bits
	if (regval & ENABLE_Z_SINGLE_CLICK_DETECTION) {
		snprintf(buf, sizeof(buf), "Z Single Click: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_Z_DOUBLE_CLICK_DETECTION) {
		snprintf(buf, sizeof(buf), "Z Double Click: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_Y_SINGLE_CLICK_DETECTION) {
		snprintf(buf, sizeof(buf), "Y Single Click: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_Y_DOUBLE_CLICK_DETECTION) {
		snprintf(buf, sizeof(buf), "Y Double Click: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_X_SINGLE_CLICK_DETECTION) {
		snprintf(buf, sizeof(buf), "X Single Click: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	if (regval & ENABLE_X_DOUBLE_CLICK_DETECTION) {
		snprintf(buf, sizeof(buf), "X Double Click: enabled\r\n"); HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
	}
	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}


void config_click_ths(uint8_t threshold, uint8_t print)
{
//	lis3dh_write_registers(CLICK_THS, &threshold, 1);
	lis3dh_i2c_write(CLICK_THS, &threshold, 1);

	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_click_ths();
	}
}

uint8_t read_click_ths(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(CLICK_THS, &regval, 1);
	return regval;
}


void print_config_click_ths(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(CLICK_THS, &regval, 1);
	lis3dh_i2c_read(CLICK_THS, &regval, 1);

	char buf[64];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- CLICK_THS -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "CLICK_THS (read) = %d\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}


void config_time_limit(uint8_t time_limit, uint8_t print)
{
//	lis3dh_write_registers(TIME_LIMIT, &time_limit, 1);
	lis3dh_i2c_write(TIME_LIMIT, &time_limit, 1);

	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_time_limit();
	}
}


uint8_t read_time_limit(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(TIME_LIMIT, &regval, 1);
	return regval;
}


void print_config_time_limit(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(TIME_LIMIT, &regval, 1);
	lis3dh_i2c_read(TIME_LIMIT, &regval, 1);

	char buf[64];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- TIME_LIMIT -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "TIME_LIMIT (read) = %d\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_time_latency(uint8_t time_latency, uint8_t print)
{
//	lis3dh_write_registers(TIME_LATENCY, &time_latency, 1);
	lis3dh_i2c_write(TIME_LATENCY, &time_latency, 1);

	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_time_latency();
	}
}

uint8_t read_time_latency(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(TIME_LATENCY, &regval, 1);
	return regval;
}


void print_config_time_latency(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(TIME_LATENCY, &regval, 1);
	lis3dh_i2c_read(TIME_LATENCY, &regval, 1);

	char buf[64];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- TIME_LATENCY -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "TIME_LATENCY (read) = %d\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}



void config_time_window(uint8_t time_window, uint8_t print)
{
//	lis3dh_write_registers(TIME_WINDOW, &time_window, 1);
	lis3dh_i2c_write(TIME_WINDOW, &time_window, 1);

	if (print == 1)
	{
		// Read back register from device to detect actual settings
		print_config_time_window();
	}
}

uint8_t read_time_window(void)
{
	uint8_t regval = 0;
	lis3dh_i2c_read(TIME_WINDOW, &regval, 1);
	return regval;
}


void print_config_time_window(void)
{
	uint8_t regval = 0;
//	lis3dh_read_registers(TIME_WINDOW, &regval, 1);
	lis3dh_i2c_read(TIME_WINDOW, &regval, 1);

	char buf[64];
	// Divider/header
	snprintf(buf, sizeof(buf), "----- TIME_WINDOW -----\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	snprintf(buf, sizeof(buf), "TIME_WINDOW (read) = %d\r\n", regval);
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	// Footer blank line
	snprintf(buf, sizeof(buf), "\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}