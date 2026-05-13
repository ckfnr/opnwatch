MCU     = atmega328p
F_CPU   = 8000000UL
CC      = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c99 \
          -Ilib/oled-display

PROG    = arduino
PORT    = /dev/ttyACM0
BAUD    = 19200

TARGET  = main

SRCS    = main.c \
          ds3231.c \
          lib/oled-display/lcd.c \
          lib/oled-display/i2c.c \
          lib/oled-display/font.c

OBJS    = $(addprefix build/, $(notdir $(SRCS:.c=.o)))

all: build $(TARGET).hex

build:
	mkdir -p build

build/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: lib/oled-display/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).hex: build/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

build/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROG) -p $(MCU) -P $(PORT) -b $(BAUD) \
	  -U flash:w:$(TARGET).hex:i

clean:
	rm -rf build $(TARGET).hex

.PHONY: all flash clean