###############################################################################
# Makefile for the project tc1
###############################################################################

## General Flags
PROJECT = tc1
MCU = atxmega128a4u
TARGET = tc1_atxmega128a4u_32.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -flto -DF_CPU=32000000UL -Os -fsigned-char $(MAKE_DEFINES)
CFLAGS += -MD -MP -MT $(*F).o -MF ./$(@D)/$(@F).d

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Os -flto


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
SRCDIR = ../..
SRC +=
ASRC +=
INC_EXCLUDES += 
UINCDIR += $(filter-out $(INC_EXCLUDES), $(filter %/, $(wildcard $(SRCDIR)/*/)))
ULIBDIR +=
ULIBS += 

SRC_EXCLUDES += ../../tools/rbf2c.c
SRC += $(filter-out $(SRC_EXCLUDES), $(wildcard $(SRCDIR)/*.c))
SRC += $(filter-out $(SRC_EXCLUDES), $(wildcard $(SRCDIR)/*/*.c))
VPATH = $(SRCDIR)

OBJECTS = $(ASRC:.s=.o) $(patsubst $(SRCDIR)/%.c,%.o,$(SRC))

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(OBJECTS) $(TARGET) tc1_atxmega128a4u_32.hex tc1_atxmega128a4u_32.eep size

%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $(INCDIR) $(patsubst %.o,%.c,$(addprefix $(SRCDIR)/, $@)) -o $@

%.o: $(addprefix $(SRCDIR)/, %.s)
	$(AS) -c $(ASMFLAGS) $< -o $@	
	
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
	-rm -rf $(OBJECTS) tc1_atxmega128a4u_32.elf ./*.d tc1_atxmega128a4u_32.hex tc1_atxmega128a4u_32.eep
	-rm -fR ./*/*.o
	-rm -fR ./*.o
	-rm -f $(PROJECT)_rom.elf $(PROJECT)_rom.map $(PROJECT)_rom.hex $(PROJECT)_rom.bin
	-rm -f $(PROJECT)_boot.elf $(PROJECT)_boot.map $(PROJECT)_boot.hex $(PROJECT)_boot.bin
	-rm -f $(PROJECT)_app.elf $(PROJECT)_app.map $(PROJECT)_app.hex $(PROJECT)_app.bin
	-rm -f $(SRC:.c=.c.bak)
	-rm -f $(SRC:.c=.lst)
	-rm -f $(ASRC:.s=.s.bak)
	-rm -f $(ASRC:.s=.lst)
	-rm -fR ./*/*.d	
	-rm -fR ./*.d
	
## Other dependencies
-include $(wildcard ./*/*.d)

