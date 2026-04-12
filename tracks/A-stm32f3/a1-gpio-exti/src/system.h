#ifndef A1_SYSTEM_H
#define A1_SYSTEM_H

#include <stdint.h>

/* SysTick configured to fire every 1 ms off the 8 MHz HSI (default
 * clock after reset — no PLL configured in A1). */
void systick_init_1ms(void);

/* Monotonic millisecond counter, incremented from SysTick_Handler. */
uint32_t millis(void);

#endif
