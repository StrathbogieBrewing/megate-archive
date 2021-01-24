###############################################################################
# Makefile for the project avr109
###############################################################################

## General Flags
PROJECT = avr109
MCU = atmega8
TARGET = avr109.elf

CC = avr-gcc
# CPP = avr-g++


## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF $(@F).d


## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -nodefaultlibs -nostartfiles  -Wl,--section-start=.text=0x1E00 -Wl,-Map=avr109.map


## Intel Hex file production flags
HEX_FLASH_FLAGS =
# -R .eeprom -R .fuse -R .lock -R .signature
#
# HEX_EEPROM_FLAGS = -j .eeprom
# HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
# HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = avr109.o

## Build
all: $(TARGET) avr109.hex avr109.lss size

## Compile
avr109.o: avr109.asm
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS)  $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

# %.eep: $(TARGET)
# 	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) avr109.elf avr109.hex  avr109.lss avr109.map $(OBJECTS).d

# default LFUSE 1 MHZ Internal RC, No brown out
LFUSE = 0xE1

# change HFUSE for 512 bytes of boot memory and move reset vector to boot memory
HFUSE = 0xDC

## AVRDude Programming
program: avr109.hex
	avrdude -C ../avrdude.conf -c dragon_isp -p m8 -P usb -e -U flash:w:avr109.hex -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m
	sleep 2
	# avrdude -C ../avrdude.conf -c dragon_isp -p m8 -P usb -U hfuse:w:0x44:m

# avrdude -c avr109 -p m8 -P /dev/ttyUSB1 -C ../avrdude.conf -b 19200 -U eeprom:r:read.hex:i
