# Embedded Security Portfolio

Hands-on projects in embedded security, trusted execution, and
post-quantum cryptography, built to deepen my practical fluency
across the software–hardware boundary that modern secure systems
are built on.

The portfolio is organised into four tracks:

- **Track A — STM32F3 Discovery** — bare-metal Cortex-M4 drivers,
  scheduling, fault handling, and a secure-boot/signed-DFU pipeline
  on Zephyr with MCUboot.
- **Track B — OP-TEE on QEMU + software post-quantum** — trusted
  applications in ARM TrustZone (QEMU ARMv8), and a hybrid
  RSA-2048 + ML-DSA signed secure boot using pqm4 on Cortex-M4.
- **Track C — Arty Z7 (Zynq-7000)** — a pair of Digilent Arty Z7-20
  boards running Linux on Cortex-A9 with user-defined logic in the
  FPGA fabric. Custom AES-128 AXI peripheral, TrustZone-partitioned
  system, mutual TLS between the two boards, and hardware-accelerated
  ML-KEM/ML-DSA via a commercial PQC IP core.
- **Track D — Reading** — concise notes on the standards and vendor
  documentation the other tracks draw from.

Each track can be read standalone, but the real story is
cross-cutting: **software ML-DSA on Cortex-M4 → software ML-DSA on
Cortex-A9 → hardware-accelerated ML-DSA in FPGA fabric**. That
three-way benchmark is what makes the case for hardware crypto
acceleration concrete.

## Coverage map

| Capability                              | Covered by                                            |
|-----------------------------------------|-------------------------------------------------------|
| C, secure programming                   | A1–A5, B2, C1–C4, C6                                  |
| Embedded hardware and drivers           | A1, A4, C1, C2, C6                                    |
| Software and computer architecture      | A1, A2, A3, C1, C5                                    |
| OS concepts                             | A2, A4, C2                                            |
| JTAG / SWD debugging                    | A1, A3, A4 (SWD); C2 (Vivado debugger)                |
| Cryptography and secure protocols       | A4, B1, B2, C1, C3, C4, C6                            |
| ARM architecture                        | all of Track A, B1, C2–C4, C6                         |
| ARM TrustZone                           | B1, C2 (real silicon)                                 |
| RISC-V architecture                     | C5 (PicoRV32 softcore)                                |
| Emulators / simulators                  | B1, B2 (QEMU)                                         |
| Hardware-accelerated crypto             | C1, C3, C6                                            |
| Post-quantum cryptography (software)    | B2                                                    |
| Post-quantum cryptography (hardware)    | C6                                                    |

## Repository layout

```
embedded-security-portfolio/
├── README.md                 # This file
├── docs/
│   ├── reading-log.md        # Track D — notes on standards and vendor docs
│   └── debug-diary.md        # Bugs hit, how they were found, what fixed them
├── tracks/
│   ├── A-stm32f3/            # STM32F3 Discovery — bare-metal + Zephyr
│   │   └── a1-gpio-exti/     # Bare-metal GPIO and EXTI driver
│   ├── B-op-tee-pqc/         # (added as projects ship)
│   └── C-arty-z7/            # (added as projects ship)
└── benchmarks/               # Cross-track performance / energy data
```

Subdirectories appear as projects ship. Empty placeholders are not
left behind on the tree — this document is the roadmap, the `tracks/`
tree is the ledger of what actually exists.

## Related repositories

- **[pqc-m4-optimization](https://github.com/danny-cagney/pqc-m4-optimization)**
  — MSc thesis work on ML-KEM (FIPS 203) performance on
  STM32F407G-DISC1 using pqm4. Corresponds to Track B2 of this
  portfolio. Private until MSc submission; will be made public
  alongside the thesis.

## Hardware

- **STM32F3 Discovery** — STM32F303VC, Cortex-M4F at 72 MHz.
  Used for all of Track A.
- **QEMU** — `qemu-system-aarch64` with the OP-TEE ARMv8 machine.
  Used for Track B1 and B2.
- **Digilent Arty Z7-20 (×2)** — Zynq-7000 dual Cortex-A9 + 7-series
  FPGA. Used for all of Track C.
- **ST-Link v2** (onboard on the Discovery) and **Digilent SmartPro
  USB** (on the Arty Z7s) for programming and debug.

## Toolchains

| Toolchain                   | Used for                               |
|-----------------------------|----------------------------------------|
| `arm-none-eabi-gcc` 13.3    | Track A (Cortex-M4)                    |
| `aarch64-linux-gnu-gcc`     | Track B (OP-TEE trusted applications)  |
| Xilinx Vivado + Vitis       | Track C (Zynq PS + PL)                 |
| OpenOCD + GDB               | SWD/JTAG debug across all hardware     |
| QEMU                        | Track B simulation                     |

## Licence

Apache 2.0 — see `LICENSE`. Vendor-supplied reference code,
datasheets, and any third-party IP retain their original licences;
see per-project READMEs for specifics.
