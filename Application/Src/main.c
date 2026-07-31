#include <stdint.h>

#include "gpio.h"
#include "uart.h"

void delay(uint32_t time)
{
	while (time--);
}

int main(void)
{
	enable_gpio();
	configure_af();
	uart_init();

	for (;;) {
		uart_write_string("hello");
		delay(4000000);
	}
}
