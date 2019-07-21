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
HEX  = $(CP) -O ihex -R .noinit -R .bss -R .vram
BIN  = $(CP) -O binary -R .noinit -R .bss -R .vram

CMSISDIR = ../../../CMSIS_5/CMSIS
OBJDIR = ./obj

# Define optimisation level here
OPT = -Ofast
LTO = -flto

# This product incorporates the ARM single-core Cortex-A9 MPCore, where the IP version is r3p0.
MCU  = cortex-a9
CORE = -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -mfpu=neon -ftree-vectorize
#CORE = -march=armv7-a -mfloat-abi=hard -mfpu=neon
#CORE = -march=armv7-a -mfloat-abi=hard -mfpu=neon -ftree-vectorize

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = $(CMSISDIR)/Core_A/Include $(CMSISDIR)/DSP/Include

# List the default directory to look for the libraries here
DLIBDIR = $(CMSISDIR)/DSP/Lib/GCC

# List all default libraries here
DLIBS = -lm -larm_cortexA9lneon_math

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

SRCDIR = ../..

# Define project name here
PROJECT = tc1_r7s721020

# Define linker script file here
LDSCRIPT_BOOT = ./r7s721020_boot.ld
LDSCRIPT_APP = ./r7s721020_app.ld

# List all user C define here, like -D_DEBUG=1
# RZ/A1H: R7S721001: CPUSTYLE_R7S721001
# RZ/A1L: R7S721020: CPUSTYLE_R7S721020
UDEFS = -DNDEBUG=1 -DCPUSTYLE_R7S721=1 -DCPUSTYLE_R7S721020=1 -DTARGET_RZA1=TARGET_RZA1L

# Define ASM defines here
UADEFS = 

# List C source files here
SRC +=

# List ASM source files here
ASRC += \
 crt_r7s721.s

# List all user directories here
UINCDIR += ../.. ../../inc ../../rza1x_inc/

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += 

SRC_EXCLUDES += ../../tools/rbf2c.c
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

MCFLAGS = $(CORE) -fno-math-errno -funroll-loops -fgraphite-identity -ffunction-sections -fdata-sections -ffat-lto-objects
#MCFLAGS = -mcpu=$(MCU) $(CORE) -fno-math-errno
#
#ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 $(ADEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mthumb-interwork -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mlong-calls -fomit-frame-pointer -Wall -Wstrict-prototypes $(DEFS)
CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -fomit-frame-pointer -Wall -Wstrict-prototypes $(DEFS) $(MAKE_DEFINES)

LDFLAGS_BOOT = $(MCFLAGS) $(LTO) $(OPT) -nostartfiles -T$(LDSCRIPT_BOOT) -Wl,-Map=$(PROJECT)_boot.map,--cref $(LIBDIR)
LDFLAGS_APP = $(MCFLAGS) $(LTO) $(OPT) -nostartfiles -T$(LDSCRIPT_APP) -Wl,-Map=$(PROJECT)_app.map,--cref $(LIBDIR)

# Generate dependency information
CPFLAGS += -MD -MP -MF ./$(@D)/$(@F).d

# Experemental optimisation flags
#CPFLAGS += -fno-inline-small-functions 
#CPFLAGS += -fno-inline-functions 
#CPFLAGS += -fwhole-program


#
# makefile rules
#

all: APP

bootloader: BOOT BOOT2

BOOT: $(OBJS) $(PROJECT)_boot.elf $(PROJECT)_boot.hex
BOOT2: $(OBJS) $(PROJECT)_boot.elf $(PROJECT)_boot.bin
APP: $(OBJS) $(PROJECT)_app.elf $(PROJECT)_app.bin

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CPFLAGS) $(INCDIR) $(addprefix $(SRCDIR)/, $(patsubst obj/%, %, $(patsubst %.o, %.c, $@))) -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) -c $(ASFLAGS) $(addprefix $(SRCDIR)/, $(patsubst obj/%, %, $(patsubst %.o, %.s, $@))) -o $@

%_boot.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS_BOOT) $(LIBS) -o $@
	$(TRGT)size --format=sysv --radix=16 $@

%_app.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS_APP) $(LIBS) -o $@
	$(TRGT)size --format=sysv --radix=16 $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN) $< $@

clean:
	-rm -fR $(OBJDIR)
	-rm -f $(PROJECT)_rom.elf $(PROJECT)_rom.map $(PROJECT)_rom.hex $(PROJECT)_rom.bin
	-rm -f $(PROJECT)_boot.elf $(PROJECT)_boot.map $(PROJECT)_boot.hex $(PROJECT)_boot.bin
	-rm -f $(PROJECT)_app.elf $(PROJECT)_app.map $(PROJECT)_app.hex $(PROJECT)_app.bin
	-rm -f $(SRC:.c=.c.bak)
	-rm -f $(SRC:.c=.lst)
	-rm -f $(ASRC:.s=.s.bak)
	-rm -f $(ASRC:.s=.lst)

# 
# Include the dependency files, should be the last of the makefile
#
-include $(wildcard $(OBJDIR)/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*/*.d)

# *** EOF ***
