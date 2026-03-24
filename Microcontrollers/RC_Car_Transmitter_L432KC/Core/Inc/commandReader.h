/*
 * commandReader.h
 *
 *  Created on: Dec 2, 2025
 *      Author: erick
 */

#ifndef INC_COMMANDREADER_H_
#define INC_COMMANDREADER_H_

#include "main.h"

void parse_uart_command(char *uart_rx_buf, char *command_out, uint8_t *indexPointer);
void parse_uart_value(char *uart_rx_buf, char *value_out, uint8_t *indexPointer);

#endif /* INC_COMMANDREADER_H_ */
