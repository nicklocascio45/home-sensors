#include "am312.h"

void am312_init(void)
{
    // Enable GPIOB peripheral clock and set mode
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= (0 << 2);

    // Enable SYSCFG peripheral
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // Select pin PB1 as source for EXTI line 1
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PB;

    // Enable interrupt for EXTI line 1
    EXTI->IMR |= EXTI_IMR_MR1;

    // Enable the rising edge trigger (motion detected)
    EXTI->RTSR |= (1 << 1);

    // Disable the falling edge trigger (motion has come to a stop)
    EXTI->FTSR &= ~(1 << 1);

    // Set priority of EXTI1 interrupt in NVIC to 3
    NVIC_SetPriority(EXTI1_IRQn, 3);

    // Enable EXTI1 interrupt in NVIC
    NVIC_EnableIRQ(EXTI1_IRQn);
}
