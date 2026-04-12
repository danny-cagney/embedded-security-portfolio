# A1 — bare-metal GPIO + EXTI on STM32F303VC.
#
# No CMake, no HAL, no CMSIS. Just arm-none-eabi-gcc, a linker script,
# and a startup file. The whole point of this project is that every
# byte in the .elf is one we put there.

CROSS    ?= arm-none-eabi-
CC       := $(CROSS)gcc
OBJCOPY  := $(CROSS)objcopy
SIZE     := $(CROSS)size
READELF  := $(CROSS)readelf

TARGET   := a1-gpio-exti
BUILD    := build
QBUILD   := build/qemu
QEMU     ?= qemu-system-arm
QEMU_MACHINE ?= netduinoplus2

CPUFLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft

CFLAGS   := $(CPUFLAGS) \
            -std=c11 -Wall -Wextra -Wpedantic -Werror \
            -Wstrict-prototypes -Wshadow -Wundef \
            -ffreestanding -fno-common -fdata-sections -ffunction-sections \
            -Os -g3 \
            -Isrc

ASFLAGS  := $(CPUFLAGS) -g3

LDSCRIPT := linker.ld
LDFLAGS  := $(CPUFLAGS) \
            -T$(LDSCRIPT) \
            -nostdlib -nostartfiles \
            -Wl,--gc-sections \
            -Wl,-Map=$(BUILD)/$(TARGET).map

C_SRCS   := src/main.c src/system.c
ASM_SRCS := startup.S

OBJS     := $(C_SRCS:%.c=$(BUILD)/%.o) $(ASM_SRCS:%.S=$(BUILD)/%.o)
DEPS     := $(OBJS:.o=.d)

ELF      := $(BUILD)/$(TARGET).elf
BIN      := $(BUILD)/$(TARGET).bin
HEX      := $(BUILD)/$(TARGET).hex

QOBJS    := $(C_SRCS:%.c=$(QBUILD)/%.o) $(ASM_SRCS:%.S=$(QBUILD)/%.o)
QDEPS    := $(QOBJS:.o=.d)
QELF     := $(QBUILD)/$(TARGET).elf

.PHONY: all clean flash debug size smoke qemu qemu-smoke

all: $(ELF) $(BIN) $(HEX) size

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -MMD -MP -c $< -o $@

$(ELF): $(OBJS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

size: $(ELF)
	@$(SIZE) $<

flash: $(ELF)
	openocd -f openocd.cfg \
	    -c "program $(ELF) verify reset exit"

debug: $(ELF)
	openocd -f openocd.cfg &
	$(CROSS)gdb $(ELF) \
	    -ex "target extended-remote localhost:3333" \
	    -ex "monitor reset halt" \
	    -ex "load"

smoke: $(ELF)
	@./tests/smoke.sh $(ELF)

# ---- QEMU variant --------------------------------------------------------
# Built with -DQEMU_SMOKE so main() exits immediately via semihosting,
# before touching any peripheral. Validates boot path only — GPIO/EXTI
# behaviour still needs real hardware.

$(QBUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DQEMU_SMOKE -MMD -MP -c $< -o $@

$(QBUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -MMD -MP -c $< -o $@

$(QELF): $(QOBJS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(QOBJS) -o $@

qemu: $(QELF)
	$(QEMU) -machine $(QEMU_MACHINE) -cpu cortex-m4 \
	    -nographic -semihosting \
	    -kernel $(QELF)

qemu-smoke: $(QELF)
	@./tests/qemu_smoke.sh $(QELF)

clean:
	rm -rf $(BUILD)

-include $(DEPS)
-include $(QDEPS)
