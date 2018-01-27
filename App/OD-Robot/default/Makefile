###############################################################################
# Makefile for the project OD-Robot
###############################################################################

## General Flags
PROJECT = OD-Robot
MCU = atmega8515
TARGET = OD-Robot.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=OD-Robot.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = OD-Robot.o ODServos.o PowerGating.o DriveServos.o ADC.o SevenSegmentDisplay.o UART-Trace.o ODSensor.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) OD-Robot.hex OD-Robot.eep OD-Robot.lss size

## Compile
OD-Robot.o: ../OD-Robot.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ODServos.o: ../ODServos.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

PowerGating.o: ../PowerGating.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

DriveServos.o: ../DriveServos.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ADC.o: ../ADC.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

SevenSegmentDisplay.o: ../SevenSegmentDisplay.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

UART-Trace.o: ../UART-Trace.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ODSensor.o: ../ODSensor.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) OD-Robot.elf dep/* OD-Robot.hex OD-Robot.eep OD-Robot.lss OD-Robot.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

