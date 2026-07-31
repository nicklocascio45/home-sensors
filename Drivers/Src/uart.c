#include "uart.h"

// OBVIOUSLY THIS ALL NEEDS TO BECOME MORE GENERIC

void uart_init(void)
{
    // Enable USART2 peripheral clock
    RCC->APB1ENR |= (1 << 17);

    // Clear all bits
    USART2->CR1 = 0x00;
    // Enable USART
    USART2->CR1 |= (1 << 13);
    // Enable transmitter
    USART2->CR1 |= (1 << 3);
    // Enable receiver
    USART2->CR1 |= (1 << 2);
    // Set baud rate to 115200 @ 16MHz
    USART2->BRR = (11 << 0) | (8 << 4);
}

void uart_write_byte(uint8_t byte)
{
    while (!(USART2->SR & (1 << 7)));
    // Set data register to byte to send
    USART2->DR = byte;
    // Wait for byte to be sent (check for TC to SET)
    while (!(USART2->SR & (1 << 6)));
}

void uart_write_string(char *string)
{
    while (*string) uart_write_byte(*string++);
}

uint8_t uart_read_byte(void)
{
    uint8_t temp;
    while (!(USART2->SR & (1 << 5)));
    temp = USART2->DR;
    return temp;
}
