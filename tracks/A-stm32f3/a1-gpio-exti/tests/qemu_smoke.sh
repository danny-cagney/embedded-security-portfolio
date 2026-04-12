#!/usr/bin/env bash
# A1 QEMU smoke test.
#
# Runs the -DQEMU_SMOKE variant of the firmware under qemu-system-arm
# on the netduinoplus2 machine (Cortex-M4, same flash/SRAM bases as
# STM32F303VC). The firmware writes a marker via ARM semihosting and
# exits. This validates: vector table load, initial SP, reset handler,
# .data copy, .bss zero, branch to main — everything except the
# hardware-specific peripheral writes.
#
# Run via `make qemu-smoke`.

set -euo pipefail

ELF="${1:-build/qemu/a1-gpio-exti.elf}"
QEMU="${QEMU:-qemu-system-arm}"
MACHINE="${QEMU_MACHINE:-netduinoplus2}"
MARKER="A1:boot-ok"
TIMEOUT_SEC=10

fail() { printf '  FAIL: %s\n' "$1" >&2; exit 1; }
pass() { printf '  ok  : %s\n' "$1"; }

[[ -f "$ELF" ]] || fail "ELF not found: $ELF (run \`make $(basename "$ELF" .elf)\` first)"
command -v "$QEMU" >/dev/null || fail "$QEMU not on PATH"

printf 'A1 QEMU smoke — %s on %s\n' "$ELF" "$MACHINE"

# macOS ships BSD timeout as `gtimeout` via coreutils, if at all.
# Fall back to a background-kill shim when neither is available.
run_with_timeout() {
    if command -v timeout >/dev/null; then
        timeout "$TIMEOUT_SEC" "$@"
    elif command -v gtimeout >/dev/null; then
        gtimeout "$TIMEOUT_SEC" "$@"
    else
        "$@" &
        local pid=$!
        ( sleep "$TIMEOUT_SEC" && kill -9 "$pid" 2>/dev/null ) &
        local killer=$!
        wait "$pid"
        local rc=$?
        kill "$killer" 2>/dev/null || true
        return $rc
    fi
}

set +e
output=$(run_with_timeout "$QEMU" \
    -machine "$MACHINE" -cpu cortex-m4 \
    -nographic -semihosting \
    -kernel "$ELF" 2>&1)
rc=$?
set -e

printf '%s\n' "$output" | sed 's/^/    | /'

(( rc == 0 )) || fail "qemu exited with status $rc"
pass "qemu exited cleanly"

grep -q "$MARKER" <<<"$output" \
    || fail "marker '$MARKER' not seen on semihosting output"
pass "semihosting marker '$MARKER' seen — reached main()"

printf 'A1 QEMU smoke test passed.\n'
