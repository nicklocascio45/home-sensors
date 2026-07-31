#include <stdint.h>

#ifndef STM32F446XX_H
#define STM32F446XX_H

/**
 * Bus base addresses
 */

#define APB1_BASEADDR		0x40000000UL
#define APB2_BASEADDR		0x40010000UL
#define AHB1_BASEADDR		0x40020000UL
#define AHB2_BASEADDR       0x50000000UL
#define AHB3_BASEADDR       0x60000000UL

/**
 * RCC definition
 */

#define RCC_BASEADDR		(AHB1_BASEADDR + 0x3800)

typedef struct
{
	volatile uint32_t CR;						/*  */
	volatile uint32_t PLL_CFGR;					/*  */
	volatile uint32_t CFGR;						/*  */
	volatile uint32_t CIR;						/*  */
	volatile uint32_t AHB1RSTR;					/*  */
	volatile uint32_t AHB2RSTR;					/*  */
	volatile uint32_t AHB3RSTR;					/*  */
	volatile uint32_t RESERVED0;				/*  */
	volatile uint32_t APB1RSTR;					/*  */
	volatile uint32_t APB2RSTR;					/*  */
	volatile uint32_t RESERVED1[2];				/*  */
	volatile uint32_t AHB1ENR;					/*  */
	volatile uint32_t AHB2ENR;					/*  */
	volatile uint32_t AHB3ENR;					/*  */
	volatile uint32_t RESERVED2;				/*  */
	volatile uint32_t APB1ENR;					/*  */
	volatile uint32_t APB2ENR;					/*  */
	volatile uint32_t RESERVED3[2];				/*  */
	volatile uint32_t AHB1LPENR;				/*  */
	volatile uint32_t AHB2LPENR;				/*  */
	volatile uint32_t AHB3LPENR;				/*  */
	volatile uint32_t RESERVED4;				/*  */
	volatile uint32_t APB1LPENR;				/*  */
	volatile uint32_t APB2LPENR;				/*  */
	volatile uint32_t RESERVED5[2];				/*  */
	volatile uint32_t BDCR;						/*  */
	volatile uint32_t CSR;						/*  */
	volatile uint32_t RESERVED6[2];				/*  */
	volatile uint32_t SSCGR;					/*  */
	volatile uint32_t PLLI2SCFGR;				/*  */
	volatile uint32_t PLLSAICFGR;				/*  */
	volatile uint32_t DCKCFGR;					/*  */
	volatile uint32_t CKGATENR;					/*  */
	volatile uint32_t DCKCFGR2;					/*  */
} RCC_RegDef_t;

#define RCC					((RCC_RegDef_t *)RCC_BASEADDR)

/**
 * EXTI definition
 */

#define EXTI_BASEADDR		(APB2_BASEADDR + 0x3C00)

typedef struct
{
	volatile uint32_t IMR;						/* Interrupt mask register */
	volatile uint32_t EMR;						/* Event mask register */
	volatile uint32_t RTSR;						/* Rising trigger selection register */
	volatile uint32_t FTSR;						/* Falling trigger selection register */
	volatile uint32_t SWIER;					/* Software interrupt event register */
	volatile uint32_t PR;						/* Pending register */
} EXTI_RegDef_t;

#define EXTI				((EXTI_RegDef_t *)EXTI_BASEADDR)

#endif // STM32F446XX_H
