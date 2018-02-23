TARGET=uart_bridge
SRC=$(wildcard *.c)
BUILD_DIR=build
CC=avr-gcc
OBJ_COPY=avr-objcopy
MCU=atmega164p
F_CPU=8000000
CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os

.PHONY: all clean program

all: $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJ_COPY) -j .text -j .data -O ihex $< $@

$(BUILD_DIR)/$(TARGET).elf: $(SRC) Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(filter %.c, $^) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

program: $(BUILD_DIR)/$(TARGET).hex
	avrdude -p m164p -c usbasp -U flash:w:$<
