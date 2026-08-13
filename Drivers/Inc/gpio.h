#include <stdint.h>

#include "stm32f446xx.h"

#ifndef GPIO_H
#define GPIO_H

/**
 * Driver APIs
 */

void enable_gpio(void);
void configure_af(void);

#endif // GPIO_H
