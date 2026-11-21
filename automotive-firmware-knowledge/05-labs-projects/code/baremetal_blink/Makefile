# Bare-metal blink – STM32F446RE (Nucleo-F446RE)
# Cài toolchain:  sudo apt install gcc-arm-none-eabi openocd
# Build:          make
# Flash:          make flash
# Debug:          make debug   (rồi ở terminal khác: arm-none-eabi-gdb build/blink.elf -ex "target extended-remote :3333")

PREFIX  = arm-none-eabi-
CC      = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
SIZE    = $(PREFIX)size

TARGET  = build/blink
SRCS    = startup.c main.c

CFLAGS  = -mcpu=cortex-m4 -mthumb -std=c99 -O0 -g3 -Wall -Wextra \
          -ffunction-sections -fdata-sections -ffreestanding
LDFLAGS = -T linker.ld -nostdlib -Wl,--gc-sections -Wl,-Map=$(TARGET).map

all: $(TARGET).elf $(TARGET).bin
	$(SIZE) $(TARGET).elf

$(TARGET).elf: $(SRCS) linker.ld
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).elf
	openocd -f board/st_nucleo_f4.cfg -c "program $(TARGET).elf verify reset exit"

debug:
	openocd -f board/st_nucleo_f4.cfg

clean:
	rm -rf build

.PHONY: all flash debug clean
