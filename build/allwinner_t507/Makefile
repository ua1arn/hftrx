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
# sun50iw9p1 Allwinner T507 (HelperBoarf T507)

TRGT2 = arm-none-eabi
#TRGT2 = aarch64-none-elf
TRGT = $(TRGT2)-

XFEL = ../../tools/xfel-windows-v1.3.2/xfel

#UADEFS += -D"WITHSMPSYSTEM"

PRJROOT := ../..
OBJDIR = obj

# Define project name here
PROJECT = tc1_awt507

# Define linker script file here
LDSCRIPT_BOOT = ./allwnr_t507_boot.ld
LDSCRIPT_APP = ./allwnr_t507_app.ld
ARCHDIR = $(PRJROOT)/arch/aw_t507

# List all user C define here, like -D_DEBUG=1
UDEFS += -D"NDEBUG"=1 -D"CPUSTYLE_T507"=1 -D"CPUSTYLE_CA53"=1
#UDEFS += -D"USE_HAL_DRIVER"=1 -D"USE_FULL_LL_DRIVER"=1
#
CC   = $(TRGT)gcc
GPP   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex -R .noinit -R .bss -R .vram -R .heap
BIN  = $(CP) -O binary -R .noinit -R .bss -R .vram -R .heap
AR   = $(TRGT)ar rcs --plugin ../libexec/gcc/$(TRGT2)/12.2.1/liblto_plugin.dll

# ARM Cortex-A53 Quad-Core (r0p4, revidr=0x80)

CORE = -mcpu=cortex-a53
CORE += -mfloat-abi=hard
#CORE += -mfpu=neon
CORE += -mfpu=neon-vfpv4
#CORE += -mfpu=vfpv4

# Define optimisation level here
#OPT = -Og
#OPT = -Ofast -flto
OPT = -Ofast

CMSISDIR = $(PRJROOT)/CMSIS_5/CMSIS
CMSISDSPDIR = $(PRJROOT)/CMSIS-DSP
DSP3DDIR = $(PRJROOT)/lib/dsp3D
USBXDIR = $(PRJROOT)/src/hal
RTOSXDIR = $(CMSISDIR)/RTOS2

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = \
	$(ARCHDIR) \
	$(USBXDIR) \
	$(DSP3DDIR) \
	$(CMSISDIR)/Core_A/Include \
	$(CMSISDIR)/Core_A/Source \
	$(RTOSXDIR)/Include \
	$(RTOSXDIR)/RTX/Include \
	$(CMSISDSPDIR)/Include \
	$(CMSISDSPDIR)/ComputeLibrary/Include \
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

# taken from
# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard = $(foreach d, $(wildcard $(1:=/*)), $(call rwildcard, $(d), $(2)) $(filter $(subst *, %, $(2)), $(d)))

SRCDIRS += $(PRJROOT)/src
#SRCDIRS += $(RTOSXDIR)/RTX/Source $(RTOSXDIR)/Source
#SRCDIRS += $(DSP3DDIR)/src
SRCDIRS += $(ARCHDIR)

ifeq (1, 0)
# lvgl
SRCDIRS += $(PRJROOT)/lvgl/src
DINCDIR += $(PRJROOT)/lvgl/src
endif

ifeq (1, 1)
# ST USB DEVICE
MDLWUSBDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Device_Library
DINCDIR += $(MDLWUSBDIR)/Core/Inc
SRCDIRS += $(MDLWUSBDIR)
endif

ifeq (1, 0)
# ST USB HOST
MDLWHOSTDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Host_Library
DINCDIR += $(MDLWHOSTDIR)/Core/Inc
SRCDIRS += $(MDLWHOSTDIR)
endif

ifeq (1, 1)
# tinyusb
# https://github.com/hathach/tinyusb.git
SRCDIRS += $(PRJROOT)/lib/hftrx_tinyusb/src
DINCDIR += $(PRJROOT)/lib/hftrx_tinyusb/src
endif

ifeq (1, 0)
# LuPng
SRCDIRS += $(PRJROOT)/lib/LuPng
DINCDIR += $(PRJROOT)/lib/LuPng
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

ifeq (1, 0)
# unzipLIB
# https://github.com/bitbank2/unzipLIB.git

UNZIPLIBDIR := $(PRJROOT)/lib/unzipLIB
DINCDIR += $(UNZIPLIBDIR)/src
SRCDIRS += $(UNZIPLIBDIR)/src

endif

ifeq (1, 0)
# FreeRTOS
RTOSDIR := $(PRJROOT)/lib/FreeRTOS-Kernel-main
DINCDIR += \
	$(RTOSDIR)/include \
	$(RTOSDIR)/portable/GCC/ARM_CA9
SRCDIRS += $(RTOSDIR)
ASRC_APP += $(call rwildcard, $(RTOSDIR)/portable/GCC/ARM_CA9, *.S)
endif

ifeq (1, 1)
# btstack
# https://github.com/bluekitchen/btstack.git
include ../btstack.mak
endif

ifeq (1, 0)
# custom application
SRCDIRS += $(PRJROOT)/lib/xhelpert507
DINCDIR += $(PRJROOT)/lib/xhelpert507
endif

CSRC += $(call rwildcard, $(SRCDIRS), *.c)
CPPSRC += $(call rwildcard, $(SRCDIRS), *.cpp)

# not all .S files in directory need
#ASRC += $(call rwildcard, $(SRCDIRS), *.S)
ASRC_APP += $(PRJROOT)/src/crt_CortexA_CPUn.S
ASRC_BOOT += $(PRJROOT)/src/crt_CortexA.S
ASRC_APP += $(ASRC_BOOT)

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
	-fno-math-errno -funroll-loops \
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
LDFLAGS_BOOT = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --sort-section=alignment -Xlinker --gc-sections -T$(LDSCRIPT_BOOT) -Wl,-Map=$(PROJECT)_boot.map,--cref $(LIBDIR) -Wl,--no-warn-rwx-segment
LDFLAGS_APP = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --sort-section=alignment -Xlinker --gc-sections -T$(LDSCRIPT_APP) -Wl,-Map=$(PROJECT)_app.map,--cref $(LIBDIR) -Wl,--no-warn-rwx-segment

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

all: APP2 $(PROJECT)_app.alw32

allZ:
	@echo Src
	@echo $(abspath $(CSRC))
	@echo Obj at $(OBJDIR)
	@echo $(abspath $(OBJS))

bootloader: BOOT2
	./egontool $(PROJECT)_boot.bin fsbl.alw32
	
# Take from tc1_t507_boot.map
SystemInitAddr_H616 = 0x0003170c
SystemInitAddr_T507 = 0x00032460

# Invoke SystemInit 
ddrinit_h616: h616_fsbl_frz.alw32
	$(XFEL) write 0x00028000 h616_fsbl_frz.alw32
	$(XFEL) exec $(SystemInitAddr_H616)

ddrinit_t507:
	$(XFEL) write 0x00028000 t507_fsbl_frz.alw32
	$(XFEL) exec $(SystemInitAddr_T507)

app_run: $(PROJECT)_app.alw32 ddrinit_t507
	$(XFEL) write 0x40000000 $(PROJECT)_app.alw32
	$(XFEL) exec 0x40000100 

# debug for bootloader - check .ld file
_app_run: bootloader
	$(XFEL) write 0x00028000 fsbl.alw32
	$(XFEL) exec 0x00028000 

flash:
	$(XFEL) spinor write 0x00000000 fsbl.alw32
	$(XFEL) spinor write 0x00040000 $(PROJECT)_app.alw32

erase:
	$(XFEL) spinor erase 0x00000000 1024

flash_old:
	$(XFEL) spinor write 0x00000000 fsbl.alw32
	$(XFEL) spinor write 0x00040000 ddr3init.bin
	$(XFEL) spinor write 0x00080000 $(PROJECT)_app.alw32

BOOT2: $(PROJECT)_boot.elf $(PROJECT)_boot.bin

APP2: $(PROJECT)_app.elf $(PROJECT)_app.bin
	
$(PROJECT)_app.alw32: $(PROJECT)_app.bin
	./stm32image -l 0x40000100 -e 0x40000100 -s $(PROJECT)_app.bin -d $@

$(OBJSC):
	@mkdir -p $(@D)
	$(CC) -o $@ -std=gnu99 -Wstrict-prototypes -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.c, $@)))

$(OBJSCPP):
	@mkdir -p $(@D)
	$(GPP) -o $@ -std=gnu++11 -fno-rtti -fexceptions -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.cpp, $@)))

$(OBJSA_APP) $(ZOBJSA):
	@mkdir -p $(@D)
	$(AS) -o $@ -c $(ASFLAGS) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.S, $@)))

# add libsun50iw9p1_spinor.a 
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
	-$(RM) $(PROJECT)_app.alw32 fsbl.alw32 $(PROJECT)_app.bin $(PROJECT)_boot.bin 

version:
	$(CC) --version
	
# 
# Include the dependency files, should be the last of the makefile
#
-include $(call rwildcard, $(OBJDIR), *.d)

# *** EOF ***
