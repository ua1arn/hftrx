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

#COMPUTEDIR = ../../../ComputeLibrary
# Add to includes
#	$(COMPUTEDIR)/arm_compute/core/NEON


CMSISDIR = ../../../CMSIS_5/CMSIS
LWIPDIR = ../../src/lwip-2.1.2/src

OBJDIR2 = obj
OBJDIR = ./$(OBJDIR2)

# Define optimisation level here
OPT = -Ofast
LTO = -flto

# This product incorporates the ARM single-core Cortex-A9 MPCore, where the IP version is r3p0.
#CORE = -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16
CORE = -mcpu=cortex-a9 -mfloat-abi=hard -mfpu=vfpv3-d16
#CORE += -mfpu=neon

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = \
	$(CMSISDIR)/Core_A/Include \
	$(CMSISDIR)/DSP/Include \
	$(CMSISDIR)/Core_A/Source \
	$(CMSISDIR)/DSP/Source \
	$(CMSISDIR)/DSP/PrivateInclude \
	$(LWIPDIR)/include

# List the default directory to look for the libraries here
DLIBDIR = $(CMSISDIR)/DSP/Lib/GCC

# List all default libraries here
DLIBS = -lm
#DLIBS = -lm -larm_cortexA9lneon_math
#DLIBS = -lm -larm_cortexA9lfdp_math

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

SRCDIR = ../../src

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
CSRC +=

# List C++ source files here
CPPSRC +=

# List ASM source files here
ASRC += \
 $(SRCDIR)/crt_r7s721.s

# List all user directories here
UINCDIR += ../.. ../../inc

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += 

#SRC_EXCLUDES += ../../tools/rbf2c.c
CSRC += $(filter-out $(SRC_EXCLUDES), $(wildcard $(SRCDIR)/*.c $(SRCDIR)/*/*.c $(SRCDIR)/*/*/*.c $(SRCDIR)/*/*/*/*.c $(SRCDIR)/*/*/*/*/*.c))
CPPSRC += $(filter-out $(SRC_EXCLUDES), $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*/*.cpp $(SRCDIR)/*/*/*.cpp $(SRCDIR)/*/*/*/*.cpp $(SRCDIR)/*/*/*/*/*.cpp))
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

MCFLAGS = $(CORE) -fno-math-errno -funroll-loops -fgraphite-identity -ffunction-sections -fdata-sections -ffat-lto-objects -ftree-vectorize
#MCFLAGS = $(CORE) -fno-math-errno
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

.PHONY: all bootloader flash clean

all: APP2 $(PROJECT)_app.nec32 app.hex

bootloader: BOOT2 boot.hex
  
app_run: IMAGE
	./dfu-util.exe --device ffff:0738 --alt 2 --transfer-size 4096 --dfuse-address 0x20000000 --download $(PROJECT)_app.nec32
	./dfu-util.exe --device ffff:0738 --alt 2 --detach

flash: IMAGE
	./dfu-util.exe --device ffff:0738 --alt 0 --dfuse-address 0x18020000 --download $(PROJECT)_app.nec32

IMAGE: $(PROJECT)_app.nec32

BOOT2: version $(OBJS) $(PROJECT)_boot.elf $(PROJECT)_boot.bin

boot.hex: $(PROJECT)_boot.bin
	./bin2ihex -l 0x00000000 -s $(PROJECT)_boot.bin >$@
	
APP2: version $(OBJS) $(PROJECT)_app.elf $(PROJECT)_app.bin

$(PROJECT)_app.nec32: $(PROJECT)_app.bin
	./stm32image.exe -l 0x20000100 -e 0x20000100 -s $(PROJECT)_app.bin -d $@

app.hex: $(PROJECT)_app.nec32
	./bin2ihex -l 0x00020000 -s $(PROJECT)_app.nec32 >$@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -std=gnu11 -Wstrict-prototypes -c $(CPFLAGS) $(INCDIR) $(addprefix $(SRCDIR)/, $(patsubst obj/%, %, $(patsubst %.o, %.c, $@))) -o $@

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

clean: version
	-rm -fR $(OBJDIR)
	-rm -f $(PROJECT)_boot.elf $(PROJECT)_boot.map
	-rm -f $(PROJECT)_app.elf $(PROJECT)_app.map

version:
	$(CC) --version
	
# 
# Include the dependency files, should be the last of the makefile
#
-include $(wildcard $(OBJDIR)/*.d)
-include $(wildcard $(OBJDIR)/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*/*/*/*.d)
-include $(wildcard $(OBJDIR)/*/*/*/*/*/*/*.d)

# *** EOF ***
