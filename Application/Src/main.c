#include <stdint.h>

#include "startup_stm32f446xx.h"
#include "gpio.h"
#include "uart.h"
#include "am312.h"

void delay(uint32_t time)
{
	while (time--);
}

int main(void)
{
	enable_gpio();
	configure_af();
	uart_init();
	am312_init();

	// Initialize extern variables
	motion_detected = 0;

	for (;;) {
		if (motion_detected) {
			// do we need to disable interrupts here??
			uart_write_string("motion detected!\n");
			motion_detected = 0;
		}
	}
}
