/* Minimal Cortex-M4 startup for STM32F303VC.
 *
 * Vector table is laid out per ARMv7-M (initial SP, then exceptions,
 * then external IRQs in interrupt-number order). Only the handlers
 * we actually use are named; the rest alias Default_Handler so a
 * stray interrupt is loud rather than silent.
 */

    .syntax unified
    .cpu cortex-m4
    .fpu softvfp
    .thumb

    .global g_pfnVectors
    .global Default_Handler

    .word _sidata
    .word _sdata
    .word _edata
    .word _sbss
    .word _ebss

/* ---- Reset handler ---------------------------------------------------- */

    .section .text.Reset_Handler
    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    ldr   r0, =_estack
    mov   sp, r0

    /* Copy .data from flash (LMA) to SRAM (VMA) */
    ldr   r0, =_sdata
    ldr   r1, =_edata
    ldr   r2, =_sidata
    movs  r3, #0
copy_data:
    cmp   r0, r1
    bcs   zero_bss
    ldr   r4, [r2, r3]
    str   r4, [r0, r3]
    adds  r3, r3, #4
    adds  r0, r0, #4
    b     copy_data

    /* Zero .bss */
zero_bss:
    ldr   r0, =_sbss
    ldr   r1, =_ebss
    movs  r2, #0
zero_loop:
    cmp   r0, r1
    bcs   call_main
    str   r2, [r0]
    adds  r0, r0, #4
    b     zero_loop

call_main:
    bl    main
hang:
    b     hang
    .size Reset_Handler, .-Reset_Handler

/* ---- Default handler -------------------------------------------------- */

    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
    b     Default_Handler
    .size Default_Handler, .-Default_Handler

/* ---- Vector table ----------------------------------------------------- */

    .section .isr_vector,"a",%progbits
    .type g_pfnVectors, %object
g_pfnVectors:
    .word _estack                /*  0: Initial SP                   */
    .word Reset_Handler          /*  1: Reset                        */
    .word NMI_Handler            /*  2: NMI                          */
    .word HardFault_Handler      /*  3: Hard fault                   */
    .word MemManage_Handler      /*  4: MPU fault                    */
    .word BusFault_Handler       /*  5: Bus fault                    */
    .word UsageFault_Handler     /*  6: Usage fault                  */
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler            /* 11: SVCall                       */
    .word DebugMon_Handler       /* 12: Debug monitor                */
    .word 0
    .word PendSV_Handler         /* 14: PendSV                       */
    .word SysTick_Handler        /* 15: SysTick                      */

    /* External interrupts — STM32F303VC, in NVIC order. Only the
     * ones we use are named; everything else is Default_Handler so
     * an unexpected source will trap visibly. */
    .word WWDG_IRQHandler                /*  0 */
    .word PVD_IRQHandler                 /*  1 */
    .word TAMP_STAMP_IRQHandler          /*  2 */
    .word RTC_WKUP_IRQHandler            /*  3 */
    .word FLASH_IRQHandler               /*  4 */
    .word RCC_IRQHandler                 /*  5 */
    .word EXTI0_IRQHandler               /*  6 — PA0 user button   */
    .word EXTI1_IRQHandler               /*  7 */
    .word EXTI2_TSC_IRQHandler           /*  8 */
    .word EXTI3_IRQHandler               /*  9 */
    .word EXTI4_IRQHandler               /* 10 */
    .size g_pfnVectors, .-g_pfnVectors

/* ---- Weak aliases ----------------------------------------------------- */

    .macro weak_alias name
    .weak \name
    .thumb_set \name, Default_Handler
    .endm

    weak_alias NMI_Handler
    weak_alias HardFault_Handler
    weak_alias MemManage_Handler
    weak_alias BusFault_Handler
    weak_alias UsageFault_Handler
    weak_alias SVC_Handler
    weak_alias DebugMon_Handler
    weak_alias PendSV_Handler
    weak_alias SysTick_Handler
    weak_alias WWDG_IRQHandler
    weak_alias PVD_IRQHandler
    weak_alias TAMP_STAMP_IRQHandler
    weak_alias RTC_WKUP_IRQHandler
    weak_alias FLASH_IRQHandler
    weak_alias RCC_IRQHandler
    weak_alias EXTI0_IRQHandler
    weak_alias EXTI1_IRQHandler
    weak_alias EXTI2_TSC_IRQHandler
    weak_alias EXTI3_IRQHandler
    weak_alias EXTI4_IRQHandler
