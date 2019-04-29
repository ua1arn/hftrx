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
#TRGT = C:\user\arm-kgp-eabi_@_x86_64-kgp-mingw32_20161016_PRUNUS\bin\arm-kgp-eabi-

#
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex -R .noinit -R .bss
BIN  = $(CP) -O binary -R .noinit -R .bss

CMSISDIR = ../../../CMSIS_5/CMSIS

# Define optimisation level here
OPT = -Ofast
LTO = -flto

# floating-point use hardware of STM32F767ZI CPU (64-bit and 32-bit float)
MCU = cortex-m7
CORE = -mthumb -mfloat-abi=hard -mfpu=fpv5-d16

#
# List all default C defines here, like -D_DEBUG=1
DDEFS =

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = $(CMSISDIR)/Core/Include $(CMSISDIR)/DSP/Include

# List the default directory to look for the libraries here
DLIBDIR = $(CMSISDIR)/DSP/Lib/GCC

# List all default libraries here
DLIBS = -lm -larm_cortexM7lfdp_math

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

SRCDIR = ../../

# Define project name here
PROJECT = tc1_stm32f767zi

# Define linker script file here
LDSCRIPT_ROM = ./STM32F767ZI_rom.ld

# List all user C define here, like -D_DEBUG=1
UDEFS = -DNDEBUG=1 -DCPUSTYLE_STM32F7XX=1 -DSTM32F767xx=1

# Define ASM defines here
UADEFS = 

# List C source files here
SRC +=

# List ASM source files here
ASRC +=

# List all user directories here
UINCDIR += ../../ ../../inc

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += 

include $(SRCDIR)/sources.mak

#
# End of user defines
##############################################################################################


INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
DEFS    = $(DDEFS) $(UDEFS)
ADEFS   = $(DADEFS) $(UADEFS)
#OBJS    = $(addprefix ../, $(ASRC:.s=.o) $(SRC:.c=.o))
OBJS    = $(ASRC:.s=.o) $(SRC:.c=.o)
LIBS    = $(DLIBS) $(ULIBS)


MCFLAGS = -mcpu=$(MCU) $(CORE) -fno-math-errno -funroll-loops -fgraphite-identity -ffunction-sections -fdata-sections -ffat-lto-objects
#MCFLAGS = -mcpu=$(MCU) $(CORE) -fno-math-errno
#
#ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 $(ADEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mthumb-interwork -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mlong-calls -fomit-frame-pointer -Wall -Wstrict-prototypes $(DEFS)
CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -fomit-frame-pointer -Wall -Wstrict-prototypes $(DEFS)

LDFLAGS_RAM = $(MCFLAGS) $(LTO) $(OPT) -nostartfiles -T$(LDSCRIPT_RAM) -Wl,-Map=$(PROJECT)_ram.map,--cref $(LIBDIR)
LDFLAGS_ROM = $(MCFLAGS) $(LTO) $(OPT) -nostartfiles -T$(LDSCRIPT_ROM) -Wl,-Map=$(PROJECT)_rom.map,--cref $(LIBDIR)

# Generate dependency information
CPFLAGS += -MD -MP -MF ./dep/$(@F).d

# Experemental optimisation flags
#CPFLAGS += -fno-inline-small-functions 
#CPFLAGS += -fno-inline-functions 
#CPFLAGS += -fwhole-program


#
# makefile rules
#

#all: RAM ROM
all: ROM

RAM: $(OBJS) $(PROJECT)_ram.elf $(PROJECT)_ram.hex

ROM: $(OBJS) $(PROJECT)_rom.elf $(PROJECT)_rom.hex
#ROM: $(OBJS) $(PROJECT)_rom.elf $(PROJECT)_rom.bin

%o: $(addprefix $(SRCDIR), %c)
	$(CC) -c $(CPFLAGS) $(INCDIR) $< -o $@

%o: $(addprefix $(SRCDIR), %s)
	$(AS) -c $(ASFLAGS) $< -o $@

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
	-rm -f $(OBJS)
	-rm -f $(PROJECT)_ram.elf
	-rm -f $(PROJECT)_ram.map
	-rm -f $(PROJECT)_ram.hex
	-rm -f $(PROJECT)_rom.elf
	-rm -f $(PROJECT)_rom.map
	-rm -f $(PROJECT)_rom.hex
	-rm -f $(SRC:.c=.c.bak)
	-rm -f $(SRC:.c=.lst)
	-rm -f $(ASRC:.s=.s.bak)
	-rm -f $(ASRC:.s=.lst)
	-rm -fR ./dep/*.d

# 
# Include the dependency files, should be the last of the makefile
#
-include $(shell mkdir ./dep 2>/dev/null) $(wildcard ./dep/*)

# *** EOF ***
