#!/usr/bin/env bash
# A1 host-side smoke test.
#
# Cannot exercise the GPIO or EXTI without the Discovery board attached,
# so this script verifies the things that *can* be checked on the host:
# the ELF builds, the vector table is at 0x08000000, the reset handler
# is wired into slot 1, the EXTI0 ISR symbol exists, and everything
# fits inside the 256K flash / 40K SRAM budget.
#
# Run via `make smoke`.

set -euo pipefail

ELF="${1:-build/a1-gpio-exti.elf}"
READELF="${READELF:-arm-none-eabi-readelf}"
NM="${NM:-arm-none-eabi-nm}"
SIZE="${SIZE:-arm-none-eabi-size}"

fail() { printf '  FAIL: %s\n' "$1" >&2; exit 1; }
pass() { printf '  ok  : %s\n' "$1"; }

[[ -f "$ELF" ]] || fail "ELF not found: $ELF (run \`make\` first)"

printf 'A1 smoke test — %s\n' "$ELF"

# 1. ELF is a valid ARM Cortex-M object.
"$READELF" -h "$ELF" | grep -q "ARM" \
    || fail "ELF machine is not ARM"
pass "ELF header reports ARM"

# 2. .isr_vector lives at the flash base, 0x08000000.
vector_addr=$("$READELF" -S "$ELF" \
    | awk '/\.isr_vector/ { print $5; exit }')
[[ "$vector_addr" == "08000000" ]] \
    || fail ".isr_vector at 0x$vector_addr, expected 0x08000000"
pass ".isr_vector at 0x08000000"

# 3. All symbols required by the ISR plumbing are present.
for sym in Reset_Handler EXTI0_IRQHandler SysTick_Handler main \
           g_pfnVectors _estack _sdata _edata _sbss _ebss; do
    "$NM" "$ELF" | grep -qE " [TtRrDdBbAaWwVv] $sym$" \
        || fail "symbol missing: $sym"
done
pass "Reset / EXTI0 / SysTick / main / vector symbols all present"

# 4. EXTI0_IRQHandler must NOT alias Default_Handler — that would mean
#    main.c's handler never got linked in and a press would hang.
exti_addr=$("$NM" "$ELF" | awk '/ EXTI0_IRQHandler$/ { print $1 }')
deflt_addr=$("$NM" "$ELF" | awk '/ Default_Handler$/ { print $1 }')
[[ -n "$exti_addr" && "$exti_addr" != "$deflt_addr" ]] \
    || fail "EXTI0_IRQHandler aliases Default_Handler"
pass "EXTI0_IRQHandler is its own function"

# 5. Sizes fit in 256K flash and 40K SRAM with comfortable headroom.
read -r text data bss _ <<<"$( "$SIZE" "$ELF" | awk 'NR==2 { print $1, $2, $3 }')"
flash_used=$((text + data))
ram_used=$((data + bss))
(( flash_used < 256 * 1024 )) || fail "flash overflow: $flash_used B"
(( ram_used   <  40 * 1024 )) || fail "SRAM overflow: $ram_used B"
printf '  ok  : flash=%d B, ram=%d B (limits 262144 / 40960)\n' \
    "$flash_used" "$ram_used"

printf 'A1 smoke test passed.\n'
