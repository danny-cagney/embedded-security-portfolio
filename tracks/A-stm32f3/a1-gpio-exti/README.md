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

*(To be filled in as the project is built. Expected layout once
working:)*

```
a1-gpio-exti/
├── README.md           # This file
├── Makefile            # arm-none-eabi-gcc, no CMake
├── openocd.cfg         # OpenOCD config for flashing + GDB server
├── linker.ld           # Minimal linker script
├── startup.S           # Vector table + reset handler
├── src/
│   ├── main.c          # GPIO/EXTI/LED logic
│   ├── system.c        # RCC / SysTick init
│   └── registers.h     # Minimal STM32F303 register definitions
└── build/              # .elf, .bin, .map — gitignored
```

Expected invocation:

```bash
make
make flash               # wraps openocd program + reset
make debug               # launches GDB attached to openocd GDB server
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

In progress. Scaffolding only at the moment — no source code
committed yet. This README is the design document.