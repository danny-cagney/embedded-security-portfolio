/* Minimal STM32F303VC register definitions.
 *
 * Hand-rolled from RM0316 rather than pulled from CMSIS — the whole
 * point of A1 is to see every offset and bit. Only what this project
 * touches is defined here.
 */

#ifndef A1_REGISTERS_H
#define A1_REGISTERS_H

#include <stdint.h>

#define __IO volatile

/* ---- RCC (RM0316 §9.4) ------------------------------------------------ */

#define RCC_BASE        0x40021000UL

typedef struct {
    __IO uint32_t CR;          /* 0x00 */
    __IO uint32_t CFGR;        /* 0x04 */
    __IO uint32_t CIR;         /* 0x08 */
    __IO uint32_t APB2RSTR;    /* 0x0C */
    __IO uint32_t APB1RSTR;    /* 0x10 */
    __IO uint32_t AHBENR;      /* 0x14 */
    __IO uint32_t APB2ENR;     /* 0x18 */
    __IO uint32_t APB1ENR;     /* 0x1C */
} RCC_TypeDef;

#define RCC             ((RCC_TypeDef *) RCC_BASE)

#define RCC_AHBENR_IOPAEN   (1U << 17)
#define RCC_AHBENR_IOPEEN   (1U << 21)
#define RCC_APB2ENR_SYSCFGEN (1U << 0)

/* ---- GPIO (RM0316 §11.4) --------------------------------------------- */

#define GPIOA_BASE      0x48000000UL
#define GPIOE_BASE      0x48001000UL

typedef struct {
    __IO uint32_t MODER;       /* 0x00 */
    __IO uint32_t OTYPER;      /* 0x04 */
    __IO uint32_t OSPEEDR;     /* 0x08 */
    __IO uint32_t PUPDR;       /* 0x0C */
    __IO uint32_t IDR;         /* 0x10 */
    __IO uint32_t ODR;         /* 0x14 */
    __IO uint32_t BSRR;        /* 0x18 */
    __IO uint32_t LCKR;        /* 0x1C */
    __IO uint32_t AFR[2];      /* 0x20-0x24 */
    __IO uint32_t BRR;         /* 0x28 */
} GPIO_TypeDef;

#define GPIOA           ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOE           ((GPIO_TypeDef *) GPIOE_BASE)

/* ---- SYSCFG (RM0316 §12.1) ------------------------------------------- */

#define SYSCFG_BASE     0x40010000UL

typedef struct {
    __IO uint32_t CFGR1;       /* 0x00 */
    __IO uint32_t RCR;         /* 0x04 */
    __IO uint32_t EXTICR[4];   /* 0x08-0x14 */
    __IO uint32_t CFGR2;       /* 0x18 */
} SYSCFG_TypeDef;

#define SYSCFG          ((SYSCFG_TypeDef *) SYSCFG_BASE)

/* ---- EXTI (RM0316 §14.3) --------------------------------------------- */

#define EXTI_BASE       0x40010400UL

typedef struct {
    __IO uint32_t IMR1;        /* 0x00 */
    __IO uint32_t EMR1;        /* 0x04 */
    __IO uint32_t RTSR1;       /* 0x08 */
    __IO uint32_t FTSR1;       /* 0x0C */
    __IO uint32_t SWIER1;      /* 0x10 */
    __IO uint32_t PR1;         /* 0x14 */
} EXTI_TypeDef;

#define EXTI            ((EXTI_TypeDef *) EXTI_BASE)

/* ---- NVIC (ARMv7-M B3.4) --------------------------------------------- */

#define NVIC_ISER_BASE  0xE000E100UL
#define NVIC_ICPR_BASE  0xE000E280UL

#define EXTI0_IRQn      6

static inline void nvic_enable_irq(uint32_t irqn)
{
    *((__IO uint32_t *)(NVIC_ISER_BASE + 4U * (irqn >> 5))) =
        (1U << (irqn & 0x1FU));
}

/* ---- SysTick (ARMv7-M B3.3) ------------------------------------------ */

#define SYSTICK_BASE    0xE000E010UL

typedef struct {
    __IO uint32_t CSR;
    __IO uint32_t RVR;
    __IO uint32_t CVR;
    __IO uint32_t CALIB;
} SysTick_TypeDef;

#define SYSTICK         ((SysTick_TypeDef *) SYSTICK_BASE)

#define SYSTICK_CSR_ENABLE     (1U << 0)
#define SYSTICK_CSR_TICKINT    (1U << 1)
#define SYSTICK_CSR_CLKSOURCE  (1U << 2)

#endif /* A1_REGISTERS_H */
