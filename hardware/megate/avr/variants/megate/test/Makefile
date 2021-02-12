###############################################################################
# Makefile for the project test
###############################################################################

## General Flags
PROJECT = test
MCU = atmega8
TARGET = test.elf

CC = avr-gcc

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
LDFLAGS +=  -Wl,-Map=test.map


## Objects that must be built in order to link
OBJECTS = test.o

## Build
all: $(TARGET) test.hex test.lss size

## Compile
test.o: test.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS)  $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $< $@

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) test.elf test.hex  test.lss test.map $(OBJECTS).d

# default LFUSE 1 MHZ Internal RC, No brown out
LFUSE = 0xE1

# change HFUSE for 512 bytes of boot memory and move reset vector to boot memory
HFUSE = 0xDC

## AVRDude Programming Bootloader
program: test.hex
	avrdude -C avrdude.conf -c stk500v2 -P usb -p m8 -e -U flash:w:test.hex -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

# read data from eeprom using bootloader
# avrdude -c avr109 -p m8 -P /dev/ttyUSB0 -C avrdude.conf -b 9600 -U eeprom:r:read.hex:i
