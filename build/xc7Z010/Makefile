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

ZINQ_BOOT_BIN := f:\\BOOT.BIN

#
CC   = $(TRGT)gcc
GPP   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex -R .noinit -R .bss -R .vram -R .heap
BIN  = $(CP) -O binary -R .noinit -R .bss -R .vram -R .heap

CMSISDIR = ../../../CMSIS_5/CMSIS
LWIPDIR = ../../src/lwip-2.1.2
# see usage of preprocessor HG_FLAT_INCLUDES and EGL_STATIC_LIBRARY symbol
OPENVGDIR = ../../src/OpenVG

OBJDIR2 = obj
OBJDIR = ./$(OBJDIR2)

# Cortex-A9 r3p0

# The Zynq-7000 family is based on the Xilinx SoC architecture. These products integrate a feature-rich dual-core or single-core ARM®
# Cortex™-A9 based processing system (PS) and 28 nm Xilinx programmable logic (PL) in a single device. The ARM Cortex-A9 CPUs are
# the heart of the PS and also include on-chip memory, external memory interfaces, and a rich set of peripheral connectivity interfaces.

CORE = -mcpu=cortex-a9 
CORE += -mfloat-abi=hard 
#CORE += -mfpu=vfpv3
#CORE += -mfpu=neon
CORE += -mfpu=neon-vfpv3
#CORE += -mlong-calls

# Define optimisation level here
# ony -Og for app
# Application
#OPT = -Og
OPT = -Ofast
LTO = -flto

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

#UADEFS += -D"WITHSMPSYSTEM"

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = \
	$(CMSISDIR)/Core_A/Include \
	$(CMSISDIR)/DSP/Include \
	$(CMSISDIR)/DSP/ComputeLibrary/Include \
	$(CMSISDIR)/Core_A/Source \
	$(CMSISDIR)/DSP/Source \
	$(CMSISDIR)/DSP/PrivateInclude \
	$(LWIPDIR)/src/include \
	$(LWIPDIR)/src/include/ipv4 \
	$(LWIPDIR)/src/apps/http \
	$(OPENVGDIR)/include/VG \
	$(OPENVGDIR)/include \
	$(OPENVGDIR)/include/EGL \
	$(OPENVGDIR)/src	\
	../../src/zynq

# List the default directory to look for the libraries here
#DLIBDIR = $(CMSISDIR)/DSP/Lib/GCC

# List all default libraries here
#DLIBS = -lm -larm_cortexA9lneon_math
#DLIBS = -lm -larm_cortexA9lfdp_math
DLIBS = -lm
#
#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

SRCDIR = ../../src

# Define project name here
PROJECT = tc1_xc7z010

# Define linker script file here
LDSCRIPT_BOOT = ./xc7z010xx_ca9_boot.ld
LDSCRIPT_APP = ./xc7z010xx_ca9_app.ld

# List all user C define here, like -D_DEBUG=1
UDEFS += -D"NDEBUG"=1 -D"CPUSTYLE_XC7Z"=1 -D"XC7Z010"=1 -D"EGL_STATIC_LIBRARY"=1

# Define ASM defines here
UADEFS += 

# List C source files here
CSRC +=

# List C++ source files here
CPPSRC +=

# List ASM source files here
ASRC += \
 $(SRCDIR)/crt_CortexA.s

# List all user directories here
UINCDIR += ../.. ../../inc

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += 

#SRC_EXCLUDES += ../../tools/rbf2c.c
CSRC += $(filter-out $(SRC_EXCLUDES), $(wildcard $(SRCDIR)/*.c $(SRCDIR)/*/*.c $(SRCDIR)/*/*/*.c $(SRCDIR)/*/*/*/*.c  $(SRCDIR)/*/*/*/*/*.c))
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
#
#ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
ASFLAGS = $(MCFLAGS) $(OPT) $(LTO) -g -gdwarf-2 $(ADEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -mthumb-interwork -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
CPFLAGS = $(MCFLAGS) $(OPT) $(LTO) -gdwarf-2 -fomit-frame-pointer -Wall $(DEFS) $(MAKE_DEFINES)

# see also -Xlinker --print-gc-sections
LDFLAGS_BOOT = $(MCFLAGS) $(LTO) $(OPT) -nostartfiles -Xlinker --gc-sections -T$(LDSCRIPT_BOOT) -Wl,-Map=$(PROJECT)_boot.map,--cref $(LIBDIR)
LDFLAGS_APP = $(MCFLAGS) $(LTO) $(OPT) -nostartfiles -Xlinker --gc-sections -T$(LDSCRIPT_APP) -Wl,-Map=$(PROJECT)_app.map,--cref $(LIBDIR)

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

all: APP2

bootloader: BOOT2
  
app_run: $(PROJECT)_app.stm32
	./dfu-util.exe --device ffff:0738 --alt 2 --transfer-size 4096 --dfuse-address 0xC0000000 --download $(PROJECT)_app.stm32
	./dfu-util.exe --device ffff:0738 --alt 2 --detach

flash: $(PROJECT)_app.stm32
	./dfu-util.exe --device ffff:0738 --alt 0 --dfuse-address 0x70080000 --download $(PROJECT)_app.stm32

BOOT2: version $(OBJS) $(PROJECT)_boot.elf BOOTL.BIN
	-cp BOOTL.BIN $(ZINQ_BOOT_BIN)

APP2: version $(OBJS) $(PROJECT)_app.elf APPL.BIN bitstream_4205.bit
	-cp APPL.BIN $(ZINQ_BOOT_BIN)

BOOTL.BIN: $(PROJECT)_boot.elf boot.bif
	bootgen -arch zynq -image boot.bif -w on -o $@

APPL.BIN: $(PROJECT)_app.elf app.bif fsbl_4205.elf
	bootgen -arch zynq -image app.bif -w on -o $@
	
$(PROJECT)_app.stm32: $(PROJECT)_app.bin
	./stm32image.exe -l 0x04100100 -e 0x04100100 -s $(PROJECT)_app.bin -d $@
	
app.hex: $(PROJECT)_app.stm32
	./bin2ihex -l 0x00080000 -s $(PROJECT)_app.stm32 >$@


$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -std=gnu11 -Wstrict-prototypes -c $(CPFLAGS) $(INCDIR) $(addprefix $(SRCDIR)/, $(patsubst $(OBJDIR2)/%, %, $(patsubst %.o, %.c, $@))) -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(GPP) -std=gnu++11 -fno-rtti -fexceptions -c $(CPFLAGS) $(INCDIR) $(addprefix $(SRCDIR)/, $(patsubst $(OBJDIR2)/%, %, $(patsubst %.o, %.cpp, $@))) -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) -c $(ASFLAGS) $(addprefix $(SRCDIR)/, $(patsubst $(OBJDIR2)/%, %, $(patsubst %.o, %.s, $@))) -o $@

%_boot.elf: $(OBJS)
	$(GPP) $(OBJS) $(LDFLAGS_BOOT) $(LIBS) -o $@
	$(SIZE) $@

%_app.elf: $(OBJS)
	$(GPP) $(OBJS) $(LDFLAGS_APP) $(LIBS) -o $@
	$(SIZE) $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN) $< $@

clean: version
	-$(RM) -R $(OBJDIR)
	-$(RM) $(PROJECT)_boot.elf $(PROJECT)_boot.map
	-$(RM) $(PROJECT)_app.elf $(PROJECT)_app.map

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
