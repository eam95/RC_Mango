/*
 * commandReader.c
 *
 *  Created on: Dec 2, 2025
 *      Author: erick
 *      The functions in this file parse commands and values from a UART received buffer.
 */
#include "delay.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "delay.h"

void parse_uart_command(char *uart_rx_buf, char *command_out, uint8_t *indexPointer)
{
	// Get the command until comma detected
	uint8_t i = 0;
	while (uart_rx_buf[i] != ',')
	{
		command_out[i] = uart_rx_buf[i];
		i++;
	}
	indexPointer[0] = i; // Return the index where comma was found
}



void parse_uart_value(char *uart_rx_buf, char *value_out, uint8_t *indexPointer)
{
	// Get the command until comma detected
	uint8_t i = indexPointer[0]+1; // Start after comma
	uint8_t j = 0;
	while (uart_rx_buf[i] != ',' && uart_rx_buf[i] != '\0')
	{
		value_out[j] = uart_rx_buf[i];
		i++;
		j++;
	}
	// Get the next index incase more parsing is needed
	indexPointer[0] = i;
}

