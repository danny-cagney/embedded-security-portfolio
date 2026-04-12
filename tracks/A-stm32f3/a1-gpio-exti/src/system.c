#include "system.h"
#include "registers.h"

/* HSI is 8 MHz on STM32F303VC at reset, and we leave it that way for
 * A1 — clock-tree configuration belongs to a later project. */
#define HSI_HZ 8000000U

static volatile uint32_t s_millis;

void systick_init_1ms(void)
{
    SYSTICK->CSR = 0;
    SYSTICK->RVR = (HSI_HZ / 1000U) - 1U;
    SYSTICK->CVR = 0;
    SYSTICK->CSR = SYSTICK_CSR_CLKSOURCE
                 | SYSTICK_CSR_TICKINT
                 | SYSTICK_CSR_ENABLE;
}

uint32_t millis(void)
{
    return s_millis;
}

void SysTick_Handler(void)
{
    s_millis++;
}
