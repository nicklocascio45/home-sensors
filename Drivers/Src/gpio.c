#include "gpio.h"

// OBVIOUSLY THIS ALL NEEDS TO BECOME MORE GENERIC

void enable_gpio(void)
{
    RCC->AHB1ENR |= (1 << 0);
}

void configure_af(void)
{
    // Set alternate function for PA2 (TX) and PA3 (RX)
    GPIOA->MODER |= (2 << 4);
    GPIOA->MODER |= (2 << 6);
    GPIOA->AFR[0] |= (7 << 8);
    GPIOA->AFR[0] |= (7 << 12);
}
