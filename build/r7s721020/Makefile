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
TRGT2 = arm-none-eabi
TRGT = $(TRGT2)-

#UADEFS += -D"WITHSMPSYSTEM"

PRJROOT := ../..
OBJDIR = obj

# Define project name here
PROJECT = tc1_r7s721020

# Define linker script file here
LDSCRIPT_BOOT0 = ./r7s721020_boot0.ld
LDSCRIPT_BOOT = ./r7s721020_boot.ld
LDSCRIPT_APP = ./r7s721020_app.ld
ARCHDIR = $(PRJROOT)/arch/renesasRZA1

# List all user C define here, like -D_DEBUG=1
UDEFS += -D"NDEBUG"=1 -DCPUSTYLE_R7S721=1 -DCPUSTYLE_R7S721020=1 -DTARGET_RZA1=TARGET_RZA1L -D"EGL_STATIC_LIBRARY"=1
UDEFS += -D"USE_HAL_DRIVER"=1 -D"USE_FULL_LL_DRIVER"=1
#
CC   = $(TRGT)gcc
GPP   = $(TRGT)g++
CP   = $(TRGT)objcopy
AR   = $(TRGT)ar rcs --plugin ../libexec/gcc/$(TRGT2)/14.2.1/liblto_plugin.dll
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

# This product incorporates the ARM single-core Cortex-A9 MPCore, where the IP version is r3p0.
#CORE = -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3 --specs=nosys.specs
CORE = -mcpu=cortex-a9 --specs=nosys.specs
CORE += -mfloat-abi=hard 
#CORE += -mfpu=vfpv3
#CORE += -mfpu=neon
CORE += -mfpu=neon-vfpv3
#CORE += -mlong-calls

# Define optimisation level here
# olny -Og for app
# Application
#OPT = -Og
OPT = -Ofast -flto

CMSISDIR = $(PRJROOT)/CMSIS_6/CMSIS
CMSISDSPDIR = $(PRJROOT)/CMSIS-DSP
# see usage of preprocessor HG_FLAT_INCLUDES and EGL_STATIC_LIBRARY symbol
USBXDIR = $(PRJROOT)/src/hal
MDLWUSBDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Device_Library
MDLWHOSTDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Host_Library

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = \
	$(ARCHDIR) \
	$(MDLWUSBDIR)/Core/Inc \
	$(MDLWHOSTDIR)/Core/Inc \
	$(USBXDIR) \
	$(CMSISDIR)/Core/Include $(CMSISDIR)/Core/Source \
	$(CMSISDSPDIR)/Include \
	$(CMSISDSPDIR)/ComputeLibrary/Include \
	$(CMSISDIR)/Core_A/Source \
	$(CMSISDSPDIR)/Source \
	$(CMSISDSPDIR)/PrivateInclude

# List the default directory to look for the libraries here
#DLIBDIR = $(CMSISDSPDIR)/Lib/GCC

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

# Define ASM defines here
UADEFS += 

# List C source files here
CSRC +=

# List C++ source files here
CPPSRC +=

# List ASM source files here
ASRC +=

# List all user directories here
UINCDIR += $(PRJROOT) $(PRJROOT)/inc

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += 

# LuPng
include ../lupng.mak

# LWIP
include ../lwip.mak

# FreeRTOS
include ../freertos.mak

# litehtml
include ../litehtml.mak

# taken from
# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard = $(foreach d, $(wildcard $(1:=/*)), $(call rwildcard, $(d), $(2)) $(filter $(subst *, %, $(2)), $(d)))

SRCDIRS += $(PRJROOT)/src
SRCDIRS += $(ARCHDIR)
SRCDIRS += $(HALDIR)
SRCDIRS += $(MDLWUSBDIR) $(MDLWHOSTDIR)

CSRC += $(call rwildcard, $(SRCDIRS), *.c)
CPPSRC += $(call rwildcard, $(SRCDIRS), *.cpp)

# not all .S files in directory need
#ASRC += $(call rwildcard, $(SRCDIRS), *.S)
#ASRC += $(PRJROOT)/src/crt_CortexA.S $(PRJROOT)/src/crt_CortexA_CPUn.S
ASRC_APP += $(PRJROOT)/src/crt_CortexA.S
ASRC_BOOT += $(ASRC_APP)

VPATH = $(PRJROOT)

#
# End of user defines
##############################################################################################


INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
DEFS    = $(DDEFS) $(UDEFS)
ADEFS   = $(DADEFS) $(UADEFS)

OBJSA_APP 	+= $(patsubst $(PRJROOT)/%.S,$(OBJDIR)/%.o, $(ASRC_APP))
OBJSA_BOOT 	+= $(patsubst $(PRJROOT)/%.S,$(OBJDIR)/%.o, $(ASRC_BOOT))
OBJSC 	+= $(patsubst $(PRJROOT)/%.c,$(OBJDIR)/%.o, $(CSRC))
OBJSCPP 	+= $(patsubst $(PRJROOT)/%.cpp,$(OBJDIR)/%.o, $(CPPSRC))

OBJS = $(OBJSC) $(OBJSCPP)

LIBS    = $(DLIBS) $(ULIBS)

MCFLAGS = \
	$(CORE) \
	-fno-math-errno \
	-funroll-loops \
	-fgraphite-identity \
	-ffunction-sections \
	-fdata-sections \
	-ffat-lto-objects \
	-ftree-vectorize
#
#ASFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -Wa,-amhls=$(<:.S=.lst) $(ADEFS)
ASFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 $(ADEFS)
#CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb-interwork -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -fomit-frame-pointer -Wall $(DEFS) $(MAKE_DEFINES)

LDCOMMONOPTS = -nostartfiles -Xlinker --sort-section=alignment -Xlinker --gc-sections
# see also -Xlinker --print-gc-sections
LDFLAGS_BOOT0 = $(MCFLAGS) $(OPT) $(LDCOMMONOPTS) -T$(LDSCRIPT_BOOT0) -Wl,-Map=$(PROJECT)_boot0.map,--cref $(LIBDIR) -Wl,--no-warn-rwx-segment
LDFLAGS_BOOT = $(MCFLAGS) $(OPT) $(LDCOMMONOPTS) -T$(LDSCRIPT_BOOT) -Wl,-Map=$(PROJECT)_boot.map,--cref $(LIBDIR) -Wl,--no-warn-rwx-segment
LDFLAGS_APP = $(MCFLAGS) $(OPT) $(LDCOMMONOPTS) -T$(LDSCRIPT_APP) -Wl,-Map=$(PROJECT)_app.map,--cref $(LIBDIR) -Wl,--no-warn-rwx-segment

# Generate dependency information
CPFLAGS += -MD -MP -MF ./$(@D)/$(@F).d
ASFLAGS += -MD -MP -MF ./$(@D)/$(@F).d

# Experemental optimisation flags
#CPFLAGS += -fno-inline-small-functions 
#CPFLAGS += -fno-inline-functions 
#CPFLAGS += -fwhole-program


#
# makefile rules
#

.PHONY: all bootloader flash clean BOOT0 BOOT2

all: APP2 $(PROJECT)_app.nec32 app.hex

bootloader: BOOT0 boot.hex

lib: BOOT2
	./bin2c $(PROJECT)_boot.bin $(PROJECT)_boot.h

app_run: IMAGE
	./dfu-util --alt 2 --transfer-size 4096 --dfuse-address 0x20000000 --download $(PROJECT)_app.nec32
	./dfu-util --alt 2 --detach

flash: IMAGE
	./dfu-util --alt 0 --dfuse-address 0x18020000 --download $(PROJECT)_app.nec32

IMAGE: $(PROJECT)_app.nec32

BOOT2: $(PROJECT)_boot.elf $(PROJECT)_boot.bin

BOOT0: $(PROJECT)_boot0.elf $(PROJECT)_boot0.bin $(PROJECT)_boot.h

boot.hex: $(PROJECT)_boot0.bin
	./bin2ihex -l 0x00000000 -s $(PROJECT)_boot0.bin >$@
	
APP2: $(PROJECT)_app.elf $(PROJECT)_app.bin

$(PROJECT)_app.nec32: $(PROJECT)_app.bin
	./stm32image -l 0x20000100 -e 0x20000100 -s $(PROJECT)_app.bin -d $@

app.hex: $(PROJECT)_app.nec32
	./bin2ihex -l 0x00020000 -s $(PROJECT)_app.nec32 >$@

$(OBJSC):
	@mkdir -p $(@D)
	$(CC) -o $@ -std=gnu99 -Wstrict-prototypes -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.c, $@)))

$(OBJSCPP):
	@mkdir -p $(@D)
	$(GPP) -o $@ -std=gnu++17 -frtti -fexceptions -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.cpp, $@)))

$(OBJSA_APP) $(ZOBJSA):
	@mkdir -p $(@D)
	$(AS) -o $@ -c $(ASFLAGS) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.S, $@)))

%_boot0.elf: $(OBJS) $(OBJSA_BOOT)
	$(GPP) $(OBJS) $(OBJSA_BOOT) $(LDFLAGS_BOOT0) $(LIBS) -o $@
	$(SIZE) $@

%_boot.elf: $(OBJS) $(OBJSA_BOOT) $(LDSCRIPT_BOOT)
	$(GPP) $(OBJS) $(OBJSA_BOOT) $(LDFLAGS_BOOT) $(LIBS) -o $@
	$(SIZE) $@

%_app.elf: $(OBJS) $(OBJSA_APP) $(LDSCRIPT_APP)
	$(GPP) $(OBJS) $(OBJSA_APP) $(LDFLAGS_APP) $(LIBS) -o $@
	$(SIZE) $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN) $< $@

clean: version
	-$(RM) -R $(OBJDIR)
	-$(RM) $(PROJECT)_boot.elf $(PROJECT)_boot.map
	-$(RM) $(PROJECT)_app.elf $(PROJECT)_app.map
	-$(RM) $(PROJECT)_app.nec32 $(PROJECT)_app.bin $(PROJECT)_boot.bin app.hex boot.hex 

version:
	$(CC) --version
	
# 
# Include the dependency files, should be the last of the makefile
#
-include $(call rwildcard, $(OBJDIR), *.d)

# *** EOF ***
