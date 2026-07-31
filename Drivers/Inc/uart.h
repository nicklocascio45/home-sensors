#include <stdint.h>

#include "stm32f446xx.h"

#ifndef UART_H
#define UART_H

/**
 * Base addresses
 */

#define USART1_BASEADDR		(APB2_BASEADDR + 0x1000)
#define USART2_BASEADDR		(APB1_BASEADDR + 0x4400)
#define USART3_BASEADDR		(APB1_BASEADDR + 0x4800)
#define UART4_BASEADDR		(APB1_BASEADDR + 0x4C00)
#define UART5_BASEADDR		(APB1_BASEADDR + 0x5000)
#define USART6_BASEADDR		(APB2_BASEADDR + 0x1400)

/**
 * UART register struct
 */

typedef struct
{
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
} USART_RegDef_t;

/**
 * UART macros
 */

#define USART1				((USART_RegDef_t *)USART1_BASEADDR)
#define USART2				((USART_RegDef_t *)USART2_BASEADDR)
#define USART3				((USART_RegDef_t *)USART3_BASEADDR)
#define UART4				((USART_RegDef_t *)UART4_BASEADDR)
#define UART5				((USART_RegDef_t *)UART5_BASEADDR)
#define USART6				((USART_RegDef_t *)USART6_BASEADDR)

/**
 * Driver APIs
 */

void uart_init(void);
void uart_write_string(char *string);

#endif // UART_H
