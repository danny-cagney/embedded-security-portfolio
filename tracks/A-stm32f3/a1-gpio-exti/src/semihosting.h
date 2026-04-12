/* ARM semihosting — just enough to report "we booted" to QEMU.
 *
 * Used only by the QEMU smoke build (see main.c, guarded by
 * QEMU_SMOKE). Real hardware never issues these — BKPT 0xAB with no
 * debugger attached would trap into HardFault.
 *
 * Reference: "Semihosting for AArch32 and AArch64" (ARM DUI 0203J).
 */

#ifndef A1_SEMIHOSTING_H
#define A1_SEMIHOSTING_H

#include <stdint.h>

#define SYS_WRITE0  0x04
#define SYS_EXIT    0x18
#define ADP_STOPPED_APPLICATION_EXIT 0x20026U

static inline uintptr_t semihost_call(uint32_t op, uintptr_t arg)
{
    register uintptr_t r0 __asm__("r0") = op;
    register uintptr_t r1 __asm__("r1") = arg;
    __asm__ volatile ("bkpt 0xAB"
                      : "+r"(r0)
                      : "r"(r1)
                      : "memory");
    return r0;
}

static inline void semihost_write0(const char *s)
{
    (void)semihost_call(SYS_WRITE0, (uintptr_t)s);
}

__attribute__((noreturn))
static inline void semihost_exit_ok(void)
{
    (void)semihost_call(SYS_EXIT, (uintptr_t)ADP_STOPPED_APPLICATION_EXIT);
    for (;;) { }
}

#endif
