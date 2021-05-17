###############################################################################
# Makefile for the project avr109
###############################################################################

## General Flags
PROJECT = m168xt4m
MCU = atmega168
TARGET = $(PROJECT).elf

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
LDFLAGS += -nodefaultlibs -nostartfiles  -Wl,--section-start=.text=0x3E00 -Wl,-Map=$(PROJECT).map


## Objects that must be built in order to link
OBJECTS = $(PROJECT).o

## Build
all: $(TARGET) $(PROJECT).hex $(PROJECT).lss size

## Compile
$(PROJECT).o: $(PROJECT).S
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
	-rm -rf $(OBJECTS) $(PROJECT).elf $(PROJECT).hex  $(PROJECT).lss $(PROJECT).map $(OBJECTS).d

LFUSE = 0xD7
HFUSE = 0xD4
LOCK = 0x0F

## AVRDude Programming Bootloader
program: $(PROJECT).hex
	avrdude -C avrdude.conf -c stk500v2 -P usb -p m168 -e -U flash:w:$(PROJECT).hex -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U lock:w:$(LOCK):m

# read data from eeprom using bootloader
# avrdude -c avr109 -p m8 -P /dev/ttyUSB0 -C avrdude.conf -b 9600 -U eeprom:r:read.hex:i

# create a file /etc/udev/rules.d/99-Custom.rules and a rules
# SUBSYSTEM=="usb", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2104", GROUP="dialout"

# relaod rules with udevadm control --reload-rules && udevadm trigger
