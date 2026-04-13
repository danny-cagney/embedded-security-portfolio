# A1 — Bare-metal GPIO and EXTI driver

Register-level C on STM32F303VC. Configure a GPIO input for the
onboard user button, route it through EXTI to an NVIC interrupt,
handle the ISR, and drive an onboard LED in response — all without
the HAL, without CMSIS device drivers, and without any RTOS
underneath.

The point of this project is not the LED. The point is to see every
register write, understand why each one is needed, and be able to
explain the ARMv7-M exception model from memory.

## Learning objectives

- Write to memory-mapped peripheral registers directly in C using
  `volatile` pointers and/or a minimal register-definition header.
- Enable a peripheral clock via RCC (Reset and Clock Control) —
  understand why this is the first thing that ever goes wrong.
- Configure a GPIO pin for input with pull-up, and another for
  push-pull output.
- Route a pin to the EXTI (External Interrupt) controller via
  SYSCFG, configure a rising/falling edge trigger, unmask the line,
  and enable the corresponding NVIC interrupt.
- Write an interrupt handler with the right linker-script name,
  acknowledge the pending bit, and toggle the LED.
- Debounce the button in software. Understand why a naive
  implementation fires the ISR dozens of times per physical press.
- Single-step the reset handler in GDB over SWD. Confirm your
  vector table is where you think it is by reading address `0x0000_0000`.

## Hardware

- **Board:** STM32F3 Discovery (STM32F303VC, Cortex-M4F @ 72 MHz)
- **Input:** user button on PA0 (active high via onboard pull-down
  — check the schematic; a common early bug is to configure the
  pin the wrong polarity)
- **Output:** LD3 blue LED on PE8 (push-pull, no pull)

## Build and flash

Toolchain: `arm-none-eabi-gcc` 13.3, `openocd` 0.12, `qemu-system-arm`
10.x. No CMake, no HAL, no CMSIS — every register touched in this
project is defined by hand in `src/registers.h`.

```
a1-gpio-exti/
├── README.md           # This file
├── Makefile            # arm-none-eabi-gcc, no CMake
├── openocd.cfg         # OpenOCD config for flashing + GDB server
├── linker.ld           # FLASH / RAM / CCM regions, vector table placement
├── startup.S           # Vector table + reset handler (.data / .bss init)
├── src/
│   ├── main.c          # GPIO / EXTI / LED logic
│   ├── system.c        # SysTick 1 ms tick, millis()
│   ├── system.h
│   ├── registers.h     # Hand-rolled STM32F303 register block
│   └── semihosting.h   # BKPT 0xAB shim, used only by the QEMU smoke build
├── tests/
│   ├── smoke.sh        # Host-side ELF checks (sections, symbols, size)
│   └── qemu_smoke.sh   # QEMU boot-path check via semihosting
└── build/              # .elf, .bin, .hex, .map — gitignored
```

Targets:

```bash
make                 # build ELF + BIN + HEX, print size
make smoke           # host-side ELF verification (no hardware, no QEMU)
make qemu-smoke      # boot-path check under qemu-system-arm netduinoplus2
make qemu            # same, interactive (raw qemu, no harness)
make flash           # openocd program + verify + reset on ST-Link/V2-B
make debug           # openocd GDB server + arm-none-eabi-gdb attach
make clean
```

## Reference material

- **STM32F303VC reference manual (RM0316)** — §6 RCC, §8 GPIO,
  §14 EXTI, §15 SYSCFG.
- **STM32F303VC datasheet** — pin/pad mapping, alternate functions.
- **ARMv7-M Architecture Reference Manual** — exception model,
  NVIC, vector table layout.
- **STM32F3 Discovery user manual (UM1570)** — which pin the user
  button and LEDs are actually on (do not guess).

## Status

Code complete, hardware bring-up pending.

The full driver is committed: register-level GPIO init, SYSCFG
routing, EXTI0 unmask, NVIC enable, and an ISR that clears `PR1`,
debounces in software against a 1 ms SysTick, and toggles PE8.
Linker script, startup file with vector table and `.data` / `.bss`
initialisation, and a hand-rolled register block are all in place.
Build is clean under `-Werror -Wall -Wextra -Wpedantic` at 420 B of
text.

Two hardware-free smoke tests gate regressions:

- **`make smoke`** — static ELF checks: vector table at `0x08000000`,
  `Reset_Handler` / `EXTI0_IRQHandler` / `SysTick_Handler` present,
  `EXTI0_IRQHandler` not aliased to `Default_Handler` (catches the
  bug where a missing ISR silently hangs on the first press), and
  flash / SRAM usage within the 256 K / 40 K budget.
- **`make qemu-smoke`** — runs a `-DQEMU_SMOKE` variant under
  `qemu-system-arm netduinoplus2` that reports via ARM semihosting
  and exits before peripheral init. Validates vector table load,
  initial SP, `.data` copy, `.bss` zero, and the branch into `main`.
  Deliberately skips GPIO / RCC / EXTI because the F405-based
  machine has different peripheral addresses than the F303 —
  peripheral behaviour is verified on real silicon.

What is **not** yet verified: the actual GPIO / EXTI / NVIC path on
the Discovery. That needs the board plugged in and `make flash`,
followed by pressing B1 and watching LD4. The single-step-the-
reset-handler exercise from the learning objectives list is the
next hardware task.