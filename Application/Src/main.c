#include "gpio.h"

void delay(uint32_t time)
{
	while (time--);
}

int main(void)
{
	RCC->AHB1ENR |= (1 << 0);
	GPIOA->MODER |= (1 << 10);

	for (;;) {
		GPIOA->BSRR = (1 << 5);
		delay(4000000);
		GPIOA->BSRR = (1 << (5 + 16));
		delay(4000000);
	}
}
