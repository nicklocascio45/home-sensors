#include <stdint.h>

#include "stm32f446xx.h"

#ifndef UART_H
#define UART_H

/**
 * Driver APIs
 */

void uart_init(void);
void uart_write_string(char *string);

#endif // UART_H
