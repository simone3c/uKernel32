CFLAGS  ?=  -std=c++23 -Isrc/include -W -Wall -Wextra -Wundef -Wshadow -Wdouble-promotion -Wunused \
	-Wformat-truncation -fno-common -Wconversion \
	-g3 -O0 -ffunction-sections -fdata-sections -I. \
	-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 $(EXTRA_CFLAGS) \
	-fno-rtti -fno-exceptions

LDFLAGS ?= -Tstm32f401re.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map

SOURCES = src/startup.cpp \
	main.cpp \
	src/gpio.cpp \
	src/systick.cpp
	
BUILD = build

all: $(BUILD)/firmware.bin

$(BUILD)/firmware.elf: $(SOURCES)
	arm-none-eabi-g++ $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	arm-none-eabi-objcopy -O binary $< $@

flash: $(BUILD)/firmware.bin
	st-flash --reset write $< 0x8000000

clean:
	rm -rf build/*
