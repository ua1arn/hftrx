###############################################################################
# Makefile for the project tc1
###############################################################################

## General Flags
PROJECT = tc1
MCU = atmega644pa
TARGET = tc1_atmega644_8.elf
CC = avr-gcc-7.1.1

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99   -flto                                                                                                                                                                                                                                                                                                                                                                                                         -DF_CPU=8000000UL -Os -fsigned-char
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Os  -flto


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = bandfilters.o display.o encoder.o hardware.o keyboard.o nvram.o spifuncs.o tc1.o formats.o sequen.o keymaps.o uc1601s.o twi.o ls020.o s1d13781.o hd44780.o board.o lph88.o pcf8535.o ili9225.o l2f50.o uc1608.o st7735.o st7565s.o ili9320.o pio.o elkey.o tests.o filters.o display2.o modems.o audio.o buffers.o serial.o synthcalcs.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) tc1_atmega644_8.hex tc1_atmega644_8.eep size

## Compile
bandfilters.o: ../bandfilters.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display.o: ../display.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

encoder.o: ../encoder.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hardware.o: ../hardware.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

keyboard.o: ../keyboard.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

nvram.o: ../nvram.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spifuncs.o: ../spifuncs.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tc1.o: ../tc1.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

formats.o: ../formats.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sequen.o: ../sequen.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

keymaps.o: ../keymaps.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uc1601s.o: ../uc1601s.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

twi.o: ../twi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ls020.o: ../ls020.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

s1d13781.o: ../s1d13781.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hd44780.o: ../hd44780.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

board.o: ../board.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lph88.o: ../lph88.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pcf8535.o: ../pcf8535.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ili9225.o: ../ili9225.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

l2f50.o: ../l2f50.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uc1608.o: ../uc1608.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

st7735.o: ../st7735.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

st7565s.o: ../st7565s.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ili9320.o: ../ili9320.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pio.o: ../pio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

elkey.o: ../elkey.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tests.o: ../tests.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

filters.o: ../filters.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display2.o: ../display2.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

modems.o: ../modems.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

audio.o: ../audio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

buffers.o: ../buffers.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

serial.o: ../serial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

synthcalcs.o: ../synthcalcs.c
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
	-rm -rf $(OBJECTS) tc1_atmega644_8.elf dep/* tc1_atmega644_8.hex tc1_atmega644_8.eep

## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

