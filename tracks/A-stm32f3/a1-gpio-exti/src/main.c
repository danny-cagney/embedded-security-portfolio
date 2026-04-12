/* A1 — bare-metal GPIO + EXTI on STM32F303VC.
 *
 * PA0  : user button (B1) — external pull-down on the Discovery,
 *        active high. Configured as input, no internal pull, routed
 *        through SYSCFG to EXTI line 0, rising-edge trigger.
 * PE8  : LD4 blue LED — push-pull output, toggled from the EXTI0
 *        ISR with a 50 ms software debounce window.
 *
 * Reference: RM0316 §9 (RCC), §11 (GPIO), §12 (SYSCFG), §14 (EXTI);
 * UM1570 §6 for the Discovery pin map.
 */

#include "registers.h"
#include "system.h"

#ifdef QEMU_SMOKE
#include "semihosting.h"
#endif

#define DEBOUNCE_MS 50U

static void gpio_init(void)
{
    /* 1. Clock the peripherals BEFORE touching any of their registers.
     *    Forgetting this is the canonical first STM32 bug. */
    RCC->AHBENR  |= RCC_AHBENR_IOPAEN | RCC_AHBENR_IOPEEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* 2. PA0 = input (MODER[1:0] = 00), no pull (PUPDR[1:0] = 00).
     *    The Discovery already has an external pull-down, so an
     *    internal pull-up here would fight it and waste current. */
    GPIOA->MODER &= ~(0x3U << (0 * 2));
    GPIOA->PUPDR &= ~(0x3U << (0 * 2));

    /* 3. PE8 = general-purpose output (MODER[17:16] = 01), push-pull
     *    (default), low speed (default), no pull (default). */
    GPIOE->MODER &= ~(0x3U << (8 * 2));
    GPIOE->MODER |=  (0x1U << (8 * 2));
}

static void exti_init(void)
{
    /* SYSCFG_EXTICR1[3:0] selects the source port for EXTI0.
     * 0000 = PA, which is the reset value, but be explicit. */
    SYSCFG->EXTICR[0] &= ~(0xFU << 0);

    EXTI->IMR1  |= (1U << 0);   /* unmask line 0                     */
    EXTI->RTSR1 |= (1U << 0);   /* trigger on rising edge (press)    */
    EXTI->FTSR1 &= ~(1U << 0);  /* not on falling edge               */

    nvic_enable_irq(EXTI0_IRQn);
}

static inline void led_toggle(void)
{
    GPIOE->ODR ^= (1U << 8);
}

int main(void)
{
#ifdef QEMU_SMOKE
    /* Host-side boot-path check: QEMU's netduinoplus2 is an STM32F405,
     * so its RCC/GPIO live at different addresses than our F303 target.
     * Touching peripherals here would prove nothing. Instead, report
     * that we reached main with the SP, .data copy and .bss zero all
     * intact, and hand control back to the harness. */
    semihost_write0("A1:boot-ok\n");
    semihost_exit_ok();
#endif

    systick_init_1ms();
    gpio_init();
    exti_init();

    /* Visible "I'm alive" — LED on at boot so a dead board is
     * distinguishable from a board that simply hasn't been pressed. */
    GPIOE->BSRR = (1U << 8);

    for (;;) {
        __asm volatile ("wfi");
    }
}

void EXTI0_IRQHandler(void)
{
    static uint32_t last_ms;
    const uint32_t now = millis();

    /* Acknowledge first — PR1 is write-1-to-clear. If we toggle the
     * LED before clearing, a slow ISR can re-enter on the same edge. */
    EXTI->PR1 = (1U << 0);

    if ((uint32_t)(now - last_ms) < DEBOUNCE_MS) {
        return;
    }
    last_ms = now;

    led_toggle();
}
