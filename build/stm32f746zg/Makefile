# $Id$
#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!    
#
##############################################################################################
# 
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#

##############################################################################################
# Start of default section
#

TRGT = arm-none-eabi-
#TRGT = C:\user\klen\bin\arm-kgp-eabi-
#
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex -R .bss -R .heap
BIN  = $(CP) -O binary -R .bss -R .heap

CMSISDIR = ../../../CMSIS_5/CMSIS
OBJDIR = ./obj

# Define optimisation level here
OPT = -Ofast
LTO = -flto

# floating-point use hardware of STM32F746ZG CPU (only 32-bit float)
MCU = cortex-m7
CORE = -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -ftree-vectorize
#CORE = -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16

#
# List all default C defines here, like -D_DEBUG=1
DDEFS =

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = \
	$(CMSISDIR)/Core/Include \
	$(CMSISDIR)/DSP/Include \
	$(CMSISDIR)/DSP/Source \
	$(CMSISDIR)/DSP/PrivateInclude

# List the default directory to look for the libraries here
#DLIBDIR = $(CMSISDIR)/DSP/Lib/GCC

# List all default libraries here
DLIBS = -lm
#DLIBS = -lm -larm_cortexM7lfdp_math

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

SRCDIR = ../../src

# Define project name here
PROJECT = tc1_stm32f746zg

# Define linker script file here
LDSCRIPT_ROM = ./STM32F746ZG_rom.ld
LDSCRIPT_RAM = ./STM32F746ZG_ram.ld

# List all user C define here, like -D_DEBUG=1
UDEFS = -DNDEBUG=1 -DCPUSTYLE_STM32F7XX=1 -DSTM32F746xx=1

# Define ASM defines here
UADEFS = 

# List C source files here
SRC +=

# List ASM source files here
ASRC +=

# List all user directories here
UINCDIR += ../.. ../../inc

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += 

#SRC_EXCLUDES += ../../tools/rbf2c.c
SRC += $(filter-out $(SRC_EXCLUDES), $(wildcard $(SRCDIR)/*.c $(SRCDIR)/*/*.c $(SRCDIR)/*/*/*.c $(SRCDIR)/*/*/*/*.c))
VPATH = $(SRCDIR)

#
# End of user defines
##############################################################################################


INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
DEFS    = $(DDEFS) $(UDEFS)
ADEFS   = $(DADEFS) $(UADEFS)
OBJS 	= $(patsubst %.s,$(OBJDIR)/%.o,$(ASRC)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
LIBS    = $(DLIBS) $(ULIBS)


MCFLAGS = -mcpu=$(MCU) $(CORE) -fno-math-errno -funroll-loops -fgraphite-identity -ffunction-sections -fdata-sections -ffat-lto-objects
#MCFLAGS = -mcpu=$(MCU) $(CORE) -fno-math-errno
#
#ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 $(ADEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mthumb-interwork -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mlong-calls -fomit-frame-pointer -Wall -Wstrict-prototypes $(DEFS)
CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -fomit-frame-pointer -Wall -Wstrict-prototypes $(DEFS) $(MAKE_DEFINES)

LDFLAGS_RAM = $(MCFLAGS) $(LTO) $(OPT) -nostartfiles -T$(LDSCRIPT_RAM) -Wl,-Map=$(PROJECT)_ram.map,--cref $(LIBDIR)
LDFLAGS_ROM = $(MCFLAGS) $(LTO) $(OPT) -nostartfiles -T$(LDSCRIPT_ROM) -Wl,-Map=$(PROJECT)_rom.map,--cref $(LIBDIR)

# Generate dependency information
CPFLAGS += -MD -MP -MF ./$(@D)/$(@F).d

# Experemental optimisation flags
#CPFLAGS += -fno-inline-small-functions 
#CPFLAGS += -fno-inline-functions 
#CPFLAGS += -fwhole-program


#
# makefile rules
#

#all: RAM ROM
all: ROM

flash: ROM
	ST-LINK_CLI.exe -Q -V -P $(PROJECT)_rom.hex -rst

RAM: $(OBJS) $(PROJECT)_ram.elf $(PROJECT)_ram.hex

ROM: $(OBJS) $(PROJECT)_rom.elf $(PROJECT)_rom.hex
#ROM: $(OBJS) $(PROJECT)_rom.elf $(PROJECT)_rom.bin

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CPFLAGS) $(INCDIR) $(addprefix $(SRCDIR)/, $(patsubst obj/%, %, $(patsubst %.o, %.c, $@))) -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) -c $(ASFLAGS) $(addprefix $(SRCDIR)/, $(patsubst obj/%, %, $(patsubst %.o, %.s, $@))) -o $@

%ram.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS_RAM) $(LIBS) -o $@
	$(TRGT)size --format=sysv --radix=16 $@

%rom.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS_ROM) $(LIBS) -o $@
	$(TRGT)size --format=sysv --radix=16 $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN) $< $@

clean:
	-rm -fR $(OBJDIR)
	-rm -f $(PROJECT)_rom.elf $(PROJECT)_rom.map $(PROJECT)_rom.hex $(PROJECT)_rom.bin
	-rm -f $(PROJECT)_ram.elf $(PROJECT)_ram.map $(PROJECT)_ram.hex $(PROJECT)_ram.bin
	$(CC) --version

# 
# Include the dependency files, should be the last of the makefile
#
-include $(wildcard $(OBJDIR)/*.d)
-include $(wildcard $(OBJDIR)/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*/*.d)


# *** EOF ***
