# Track A — STM32F3 Discovery

Hands-on Cortex-M4 work on the STM32F3 Discovery board
(STM32F303VC at 72 MHz). The goal is to prove hardware
understanding bare-metal first — driver, scheduler, fault handler
from raw registers — and then mirror a realistic embedded product
flow with Zephyr and MCUboot for the signed-DFU headline project.

No HAL for A1–A3. Zephyr for A4 onward.

## Project list

| #  | Project                                                      | Size    | Status      |
|----|--------------------------------------------------------------|---------|-------------|
| A1 | [Bare-metal GPIO and EXTI driver](a1-gpio-exti/)             | Weekend | In progress |
| A2 | Bare-metal cooperative scheduler with priority inversion     | Weekend | Planned     |
| A3 | Custom HardFault handler, bare-metal                         | Weekend | Planned     |
| A4 | MCUboot + signed DFU over UART, on Zephyr *(Track headline)* | 2 weeks | Planned     |
| A5 | Buffer overflow exploit lab, bare-metal *(optional)*         | Weekend | Planned     |

## Hardware

- **Board:** STM32F3 Discovery (STM32F303VC, Cortex-M4F at 72 MHz,
  256 KB flash, 40 KB SRAM).
- **Debugger:** ST-Link v2 onboard, connected over SWD.
- **Peripherals used:** onboard blue user button (PA0), onboard
  LEDs (PE8–PE15), USART on PA9/PA10 broken out via USB-UART
  adapter.

## Toolchain

- `arm-none-eabi-gcc` 13.3 (installed at
  `~/tools/arm-gnu-toolchain-13.3.rel1-darwin-arm64-arm-none-eabi/`
  for macOS; adjust for Linux). Toolchain pin is deliberate — newer
  GCC 15.x is stricter about Thumb interworking relocations and
  breaks some bare-metal and Zephyr code paths.
- OpenOCD 0.12 for flashing and SWD.
- GDB (shipped with the GNU toolchain) for step debug.
- CLion as editor, using its Embedded Development plugin for
  OpenOCD run configurations.

## Why bare-metal first

The plan deliberately starts register-level. Any question
about memory-mapped peripherals, interrupt priority, the ARMv7-M
exception model, or bring-up debug is much easier to answer once
you have physically written to `GPIOA->MODER` and single-stepped
the reset handler yourself. The Zephyr-based A4 project then shows
the productised version of the same ideas, which is what a
real-world secure embedded product ships.
