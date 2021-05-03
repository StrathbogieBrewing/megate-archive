###############################################################################
# Makefile for the project avr109
###############################################################################

## General Flags
PROJECT = jackal
MCU = atmega168
TARGET = jackal.elf

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
LDFLAGS += -nodefaultlibs -nostartfiles  -Wl,--section-start=.text=0x3E00 -Wl,-Map=jackal.map


## Objects that must be built in order to link
OBJECTS = jackal.o

## Build
all: $(TARGET) jackal.hex jackal.lss size

## Compile
jackal.o: jackal.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

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
	-rm -rf $(OBJECTS) jackal.elf jackal.hex  jackal.lss jackal.map $(OBJECTS).d

# default LFUSE 1 MHZ Internal RC, No brown out
LFUSE = 0xE1

# change HFUSE for 512 bytes of boot memory and move reset vector to boot memory
# enable CKOPT
HFUSE = 0xCC

LOCK = 0xEF

## AVRDude Programming Bootloader
program: jackal.hex
	avrdude -C avrdude.conf -c stk500v2 -P usb -p m168 -e -U flash:w:jackal.hex -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U lock:w:$(LOCK):m

# read data from eeprom using bootloader
# avrdude -c avr109 -p m8 -P /dev/ttyUSB0 -C avrdude.conf -b 9600 -U eeprom:r:read.hex:i

# create a file /etc/udev/rules.d/99-Custom.rules and a rules
# SUBSYSTEM=="usb", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2104", GROUP="dialout"

# relaod rules with udevadm control --reload-rules && udevadm trigger
