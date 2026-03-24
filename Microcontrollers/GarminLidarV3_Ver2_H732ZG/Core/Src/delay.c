#include "delay.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "GarminLidarLiteV3.h"
#include "main.h"
/**
 * @brief Delays execution for a specified number of microseconds.
 * @param us: The number of microseconds to delay.
 */

extern TIM_HandleTypeDef htim24;

void delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim24, 0); // Reset the counter
    HAL_TIM_Base_Start(&htim24);      // Start the timer
    while (__HAL_TIM_GET_COUNTER(&htim24) < us)
    {
        // Wait until the counter reaches the desired delay
    }
    HAL_TIM_Base_Stop(&htim24);       // Stop the timer
}



uint32_t adjusteTimeForTicks(uint32_t time_us)
{
	// Adjust time in us to timer ticks (10ns per tick with prescalar of 25 on 250MHz clock)
	// Each tick is 10ns, so 1us = 100 ticks
	return 10*(time_us -1);
}

