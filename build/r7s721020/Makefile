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
AR   = $(TRGT)ar rcs --plugin ../libexec/gcc/$(TRGT2)/12.2.1/liblto_plugin.dll
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex -R .noinit -R .bss -R .vram -R .heap
BIN  = $(CP) -O binary -R .noinit -R .bss -R .vram -R .heap

# This product incorporates the ARM single-core Cortex-A9 MPCore, where the IP version is r3p0.
#CORE = -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3
CORE = -mcpu=cortex-a9 
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

CMSISDIR = $(PRJROOT)/CMSIS_5/CMSIS
CMSISDSPDIR = $(PRJROOT)/CMSIS-DSP
LWIPDIR = $(PRJROOT)/lib/lwip-2.1.2
# see usage of preprocessor HG_FLAT_INCLUDES and EGL_STATIC_LIBRARY symbol
OPENVGDIR = $(PRJROOT)/lib/OpenVG
DSP3DDIR = $(PRJROOT)/lib/dsp3D
USBXDIR = $(PRJROOT)/src/hal
MDLWUSBDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Device_Library
MDLWHOSTDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Host_Library
RTOSXDIR = $(CMSISDIR)/RTOS2
LUPNGDIR = $(PRJROOT)/lib/LuPng

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
	$(DSP3DDIR) \
	$(CMSISDIR)/Core_A/Include \
	$(RTOSXDIR)/Include \
	$(RTOSXDIR)/RTX/Include \
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


ifeq (1, 0)
# LuPng
SRCDIRS += $(LUPNGDIR)
DINCDIR += $(LUPNGDIR)
endif

ifeq (1, 0)
# OpenVG
# see usage of preprocessor HG_FLAT_INCLUDES and EGL_STATIC_LIBRARY symbol
OPENVGDIR = $(PRJROOT)/lib/OpenVG
DINCDIR += \
	$(OPENVGDIR)/include/VG \
	$(OPENVGDIR)/include \
	$(OPENVGDIR)/include/EGL \
	$(OPENVGDIR)/src
SRCDIRS += $(OPENVGDIR)/src
UDEFS += -D"EGL_STATIC_LIBRARY"=1
endif

ifeq (1, 0)
# LWIP
LWIPDIR = $(PRJROOT)/lib/lwip-2.1.2
DINCDIR += \
	$(LWIPDIR)/src/include \
	$(LWIPDIR)/src/include/ipv4 \
	$(LWIPDIR)/src/apps/http
SRCDIRS += $(LWIPDIR)/src $(PRJROOT)/lib/dhcp-server $(PRJROOT)/lib/dns-server
endif

# taken from
# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard = $(foreach d, $(wildcard $(1:=/*)), $(call rwildcard, $(d), $(2)) $(filter $(subst *, %, $(2)), $(d)))

#SRCDIRS = $(PRJROOT)/src $(HALDIR) $(MDLWUSBDIR) $(MDLWHOSTDIR) $(OPENVGDIR)/src $(LWIPDIR)/src $(DSP3DDIR)/src
SRCDIRS += $(PRJROOT)/src
#SRCDIRS += $(RTOSXDIR)/RTX/Source $(RTOSXDIR)/Source
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

OBJSA_APP 	+= $(patsubst $(PRJROOT)/%.S, $(OBJDIR)/%.o, $(ASRC_APP))
OBJSA_BOOT 	+= $(patsubst $(PRJROOT)/%.S, $(OBJDIR)/%.o, $(ASRC_BOOT))
OBJSC 	+= $(patsubst $(PRJROOT)/%.c, $(OBJDIR)/%.o, $(CSRC))
OBJSCPP 	+= $(patsubst $(PRJROOT)/%.cpp, $(OBJDIR)/%.o, $(CPPSRC))

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

# see also -Xlinker --print-gc-sections
LDFLAGS_BOOT = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --gc-sections -T$(LDSCRIPT_BOOT) -Wl,-Map=$(PROJECT)_boot.map,--cref $(LIBDIR) -Wl,--no-warn-rwx-segment
LDFLAGS_APP = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --gc-sections -T$(LDSCRIPT_APP) -Wl,-Map=$(PROJECT)_app.map,--cref $(LIBDIR) -Wl,--no-warn-rwx-segment

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

.PHONY: all bootloader flash clean

all: APP2 $(PROJECT)_app.nec32 app.hex

bootloader: BOOT2 boot.hex
  
app_run: IMAGE
	./dfu-util --alt 2 --transfer-size 4096 --dfuse-address 0x20000000 --download $(PROJECT)_app.nec32
	./dfu-util --alt 2 --detach

flash: IMAGE
	./dfu-util --alt 0 --dfuse-address 0x18020000 --download $(PROJECT)_app.nec32

IMAGE: $(PROJECT)_app.nec32

BOOT2: $(PROJECT)_boot.elf $(PROJECT)_boot.bin

boot.hex: $(PROJECT)_boot.bin
	./bin2ihex -l 0x00000000 -s $(PROJECT)_boot.bin >$@
	
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
	$(GPP) -o $@ -std=gnu++11 -fno-rtti -fexceptions -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.cpp, $@)))

$(OBJSA_APP) $(ZOBJSA):
	@mkdir -p $(@D)
	$(AS) -o $@ -c $(ASFLAGS) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.S, $@)))

%_boot.elf: $(OBJS) $(OBJSA_BOOT)
	$(GPP) $(OBJS) $(OBJSA_BOOT) $(LDFLAGS_BOOT) $(LIBS) -o $@
	$(SIZE) $@

%_app.elf: $(OBJS) $(OBJSA_APP)
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

version:
	$(CC) --version
	
# 
# Include the dependency files, should be the last of the makefile
#
-include $(call rwildcard, $(OBJDIR), *.d)

# *** EOF ***
