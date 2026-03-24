#ifndef DELAY_H
#define DELAY_H

#include "main.h"

/**
 * @brief Delays execution for a specified number of microseconds.
 * @param us: The number of microseconds to delay.
 */

void delay_us(uint32_t ticks);
uint32_t adjusteTimeForTicks(uint32_t time_us);

#endif // DELAY_H
