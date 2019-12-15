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
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex -R .noinit -R .bss -R .vram -R .heap
BIN  = $(CP) -O binary -R .noinit -R .bss -R .vram -R .heap

CMSISDIR = ../../../CMSIS_5/CMSIS
OBJDIR2 = obj
OBJDIR = ./$(OBJDIR2)

# Define optimisation level here
OPT = -Ofast
LTO = -flto

# This product incorporates the ARM single-core Cortex-A9 MPCore, where the IP version is r3p0.
MCU  = cortex-a7
CORE = -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4-d16 -ftree-vectorize
#CORE = -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4-d16 -mfpu=neon -ftree-vectorize

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = $(CMSISDIR)/Core_A/Include $(CMSISDIR)/DSP/Include

# List the default directory to look for the libraries here
DLIBDIR = $(CMSISDIR)/DSP/Lib/GCC

# List all default libraries here
#DLIBS = -lm -larm_cortexA9lneon_math
DLIBS = -lm -larm_cortexA9lfdp_math

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

SRCDIR = ../..

# Define project name here
PROJECT = stm32mp157axx

# Define linker script file here
#LDSCRIPT_BOOT = ./stm32mp15xx_cm7_boot.ld
#LDSCRIPT_APP = ./stm32mp15xx_cm7_app.ld
LDSCRIPT_APP = ./stm32mp15xx_cm7_boot.ld

# List all user C define here, like -D_DEBUG=1
UDEFS = -DNDEBUG=1 -DCPUSTYLE_STM32MP1=1 -DCPUSTYLE_STM32MP157A=1 -DSTM32MP157Axx=1

# Define ASM defines here
UADEFS = 

# List C source files here
CSRC +=

# List C++ source files here
CPPSRC +=

# List ASM source files here
ASRC += \
 $(SRCDIR)/crt_r7s721.s

# List all user directories here
UINCDIR += ../.. ../../inc ../../rza1x_inc/

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += 

SRC_EXCLUDES += ../../tools/rbf2c.c
CSRC += $(filter-out $(SRC_EXCLUDES), $(wildcard $(SRCDIR)/*.c $(SRCDIR)/*/*.c $(SRCDIR)/*/*/*.c $(SRCDIR)/*/*/*/*.c))
CPPSRC += $(filter-out $(SRC_EXCLUDES), $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*/*.cpp $(SRCDIR)/*/*/*.cpp $(SRCDIR)/*/*/*/*.cpp))
VPATH = $(SRCDIR)

#
# End of user defines
##############################################################################################


INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
DEFS    = $(DDEFS) $(UDEFS)
ADEFS   = $(DADEFS) $(UADEFS)
OBJS 	+= $(patsubst $(SRCDIR)/%.s,$(OBJDIR)/%.o,$(ASRC))
OBJS 	+= $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CSRC))
OBJS 	+= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPPSRC))
LIBS    = $(DLIBS) $(ULIBS)

MCFLAGS = $(CORE) -fno-math-errno -funroll-loops -fgraphite-identity -ffunction-sections -fdata-sections -ffat-lto-objects
#MCFLAGS = -mcpu=$(MCU) $(CORE) -fno-math-errno
#
#ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 $(ADEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mthumb-interwork -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mlong-calls -fomit-frame-pointer -Wall $(DEFS)
CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -fomit-frame-pointer -Wall $(DEFS) $(MAKE_DEFINES)

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

all: APP2 fsbl.stm32 app.hex

bootloader: BOOT2

BOOT: $(OBJS) $(PROJECT)_boot.elf $(PROJECT)_boot.hex
BOOT2: $(OBJS) $(PROJECT)_boot.elf $(PROJECT)_boot.bin
APP: $(OBJS) $(PROJECT)_app.elf $(PROJECT)_app.bin
APP2: $(OBJS) $(PROJECT)_app.elf $(PROJECT)_app.hex

fsbl.stm32: $(PROJECT)_app.bin
	./stm32image.exe -l 2FFC2500 -e 2FFC2500 -s $(PROJECT)_app.bin -d ./fsbl.stm32
	
app.hex: fsbl.stm32
	./bin2ihex fsbl.stm32 >app.hex


$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -std=gnu11 -Wstrict-prototypes -c $(CPFLAGS) $(INCDIR) $(addprefix $(SRCDIR)/, $(patsubst $(OBJDIR2)/%, %, $(patsubst %.o, %.c, $@))) -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -std=gnu++11 -fno-rtti -c $(CPFLAGS) $(INCDIR) $(addprefix $(SRCDIR)/, $(patsubst $(OBJDIR2)/%, %, $(patsubst %.o, %.cpp, $@))) -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) -c $(ASFLAGS) $(addprefix $(SRCDIR)/, $(patsubst $(OBJDIR2)/%, %, $(patsubst %.o, %.s, $@))) -o $@

%_boot.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS_BOOT) $(LIBS) -o $@
	$(SIZE) $@

%_app.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS_APP) $(LIBS) -o $@
	$(SIZE) $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN) $< $@


clean:
	-rm -fR $(OBJDIR)
	-rm -f $(PROJECT)_boot.elf $(PROJECT)_boot.map
	-rm -f $(PROJECT)_app.elf $(PROJECT)_app.map
	
clean2:
	-rm -fR $(OBJDIR)
	-rm -f $(PROJECT)_boot.elf $(PROJECT)_boot.map $(PROJECT)_boot.hex $(PROJECT)_boot.bin
	-rm -f $(PROJECT)_app.elf $(PROJECT)_app.map $(PROJECT)_app.hex $(PROJECT)_app.bin


# 
# Include the dependency files, should be the last of the makefile
#
-include $(wildcard $(OBJDIR)/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*/*.d)

# *** EOF ***
