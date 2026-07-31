#include <stdint.h>

#include "stm32f446xx.h"

/**
 * Base addresses of each GPIO
 */

#define GPIOA_BASEADDR		(AHB1_BASEADDR + 0x0000)
#define GPIOB_BASEADDR		(AHB1_BASEADDR + 0x0400)
#define GPIOC_BASEADDR		(AHB1_BASEADDR + 0x0800)
#define GPIOD_BASEADDR		(AHB1_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR		(AHB1_BASEADDR + 0x1000)
#define GPIOF_BASEADDR		(AHB1_BASEADDR + 0x1400)
#define GPIOG_BASEADDR		(AHB1_BASEADDR + 0x1800)
#define GPIOH_BASEADDR		(AHB1_BASEADDR + 0x1C00)

/**
 * GPIO register struct
 */

typedef struct 
{
	volatile uint32_t MODER;				/*  */
	volatile uint32_t OTYPER;				/*  */
	volatile uint32_t OSPEEDR;				/*  */
	volatile uint32_t PUPDR;				/*  */
	volatile uint32_t IDR;					/*  */
	volatile uint32_t ODR;					/*  */
	volatile uint32_t BSRR;					/*  */
	volatile uint32_t LCKR;					/*  */
	volatile uint32_t AFR[2];				/*  */
} GPIO_RegDef_t;

/**
 * GPIO macros
 */

#define GPIOA				((GPIO_RegDef_t *)GPIOA_BASEADDR)
#define GPIOB	    		((GPIO_RegDef_t *)GPIOB_BASEADDR)
#define GPIOC				((GPIO_RegDef_t *)GPIOC_BASEADDR)
#define GPIOD				((GPIO_RegDef_t *)GPIOD_BASEADDR)
#define GPIOE				((GPIO_RegDef_t *)GPIOE_BASEADDR)
#define GPIOF				((GPIO_RegDef_t *)GPIOF_BASEADDR)
#define GPIOG				((GPIO_RegDef_t *)GPIOG_BASEADDR)
#define GPIOH				((GPIO_RegDef_t *)GPIOH_BASEADDR)

/**
 * Driver APIs
 */


