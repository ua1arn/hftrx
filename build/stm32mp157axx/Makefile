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
#TRGT2 = C:\user\klen\bin\arm-kgp-eabi
TRGT = $(TRGT2)-

DFUUTIL = ./dfu-util

#UADEFS += -D"WITHSMPSYSTEM"

PRJROOT := ../..
OBJDIR = o

# Define project name here
PROJECT = tc1_stm32mp157axx

# Define linker script file here
LDSCRIPT_BOOT = ./stm32mp15xx_ca7_boot.ld
LDSCRIPT_APP = ./stm32mp15xx_ca7_app.ld
ARCHDIR = $(PRJROOT)/arch/stm32mp1xx

# List all user C define here, like -D_DEBUG=1
UDEFS += -D"NDEBUG"=1 -D"CPUSTYLE_STM32MP1"=1
UDEFS += -D"STM32MP157Axx"=1 -D"STM32MP1"=1 -D"CORE_CA7"=1 -D"USE_HAL_DRIVER"=1 -D"USE_FULL_LL_DRIVER"=1
#
CC   = $(TRGT)gcc
GPP   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary
AR   = $(TRGT)ar rcs --plugin ../libexec/gcc/$(TRGT2)/14.2.1/liblto_plugin.dll

# This product incorporates the ARM dual-core Cortex-A7 MPCore, where the IP version is r0p5
# debug architecture ARMv7.1.
CORE = -mcpu=cortex-a7 --specs=nosys.specs
CORE += -mfloat-abi=hard
#CORE += -mfpu=neon
CORE += -mfpu=neon-vfpv4
#CORE += -mfpu=vfpv4

# Define optimisation level here
#OPT = -Og
#OPT = -Os
#OPT = -Ofast
OPT = -Ofast -flto

CMSISDIR = $(PRJROOT)/CMSIS_6/CMSIS
CMSISDSPDIR = $(PRJROOT)/CMSIS-DSP
USBXDIR = $(PRJROOT)/src/hal
MDLWUSBDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Device_Library
MDLWHOSTDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Host_Library
HALDIR = $(PRJROOT)/lib/Drivers/STM32MP1xx_HAL_Driver
RTOSXDIR = $(CMSISDIR)/RTOS2

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = \
	$(ARCHDIR) \
	$(HALDIR)/Inc \
	$(USBXDIR) \
	$(CMSISDIR)/Core/Include $(CMSISDIR)/Core/Source \
	$(RTOSXDIR)/Include \
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

# taken from
# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard = $(foreach d, $(wildcard $(1:=/*)), $(call rwildcard, $(d), $(2)) $(filter $(subst *, %, $(2)), $(d)))

SRCDIRS += $(PRJROOT)/src
#SRCDIRS += $(RTOSXDIR)/RTX/Source $(RTOSXDIR)/Source

# lvgl
include ../lvgl.mak

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
TINYUSBDIR = $(PRJROOT)/lib/hftrx_tinyusb
SRCDIRS += $(TINYUSBDIR)/src
DINCDIR += $(TINYUSBDIR)/src
endif

# LuPng
include ../lupng.mak

# LWIP
include ../lwip.mak

# FreeRTOS
include ../freertos.mak

# btstack
include ../btstack.mak

# litehtml
include ../litehtml.mak

ifeq (1, 0)
# litehtml is the lightweight HTML rendering engine with CSS2/CSS3 support
# http://www.litehtml.com/
LITEHTMLDIR = $(PRJROOT)/lib/litehtml
DINCDIR += $(LITEHTMLDIR)/include $(LITEHTMLDIR)/src/gumbo/include $(LITEHTMLDIR)/include/litehtml $(LITEHTMLDIR)/src/gumbo/include/gumbo
SRCDIRS += $(LITEHTMLDIR)/src
UDEFS += -D"LITEHTML_NO_THREADS"=1
endif

SRCDIRS += $(ARCHDIR)
SRCDIRS += $(HALDIR)

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

OBJSA_APP 	+= $(patsubst $(PRJROOT)/%.S,$(OBJDIR)/%.o, $(ASRC_APP))
OBJSA_BOOT 	+= $(patsubst $(PRJROOT)/%.S,$(OBJDIR)/%.o, $(ASRC_BOOT))
OBJSC 	+= $(patsubst $(PRJROOT)/%.c,$(OBJDIR)/%.o, $(CSRC))
OBJSCPP 	+= $(patsubst $(PRJROOT)/%.cpp,$(OBJDIR)/%.o, $(CPPSRC))

OBJS = $(OBJSC) $(OBJSCPP)

LIBS    = $(DLIBS) $(ULIBS)

MCFLAGS = \
	$(CORE) \
	-fno-math-errno -funroll-loops \
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
LDFLAGS_BOOT = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --sort-section=alignment -Xlinker --gc-sections -T$(LDSCRIPT_BOOT) -Wl,-Map=$(PROJECT)_boot.map,--cref $(LIBDIR) $(call ld-option, --no-warn-rwx-segments)
LDFLAGS_APP = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --sort-section=alignment -Xlinker --gc-sections -T$(LDSCRIPT_APP) -Wl,-Map=$(PROJECT)_app.map,--cref $(LIBDIR) $(call ld-option, --no-warn-rwx-segments)

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

all: APP2 $(PROJECT)_app.stm32 app.hex

allZ:
	@echo Src
	@echo $(abspath $(CSRC))
	@echo Obj at $(OBJDIR)
	@echo $(abspath $(OBJS))

bootloader: BOOT2 fsbl.stm32 boot.hex
  
app_run: $(PROJECT)_app.stm32
	$(DFUUTIL) --alt 2 --transfer-size 4096 --dfuse-address 0xC0000000 --download "tc1_stm32mp157axx_app.stm32"
	$(DFUUTIL) --alt 2 --detach

flash: $(PROJECT)_app.stm32
	$(DFUUTIL) --alt 0 --dfuse-address 0x70080000 --download $(PROJECT)_app.stm32

BOOT2: $(PROJECT)_boot.elf $(PROJECT)_boot.bin

APP2: $(PROJECT)_app.elf $(PROJECT)_app.bin

fsbl.stm32: $(PROJECT)_boot.bin
	./stm32image -l 0x2FFC2500 -e 0x2FFC2500 -s $(PROJECT)_boot.bin -d $@
	
boot.hex: fsbl.stm32
	./bin2ihex -l 0x00000000 -s fsbl.stm32 >$@
	
$(PROJECT)_app.stm32: $(PROJECT)_app.bin
	./stm32image -l 0xC0000100 -e 0xC0000100 -s $(PROJECT)_app.bin -d $@
	
app.hex: $(PROJECT)_app.stm32
	./bin2ihex -l 0x00080000 -s $(PROJECT)_app.stm32 >$@

$(OBJSC):
	@mkdir -p $(@D)
	$(CC) -o $@ -std=gnu99 -Wstrict-prototypes -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.c, $@)))

$(OBJSCPP):
	@mkdir -p $(@D)
	$(GPP) -o $@ -std=gnu++17 -frtti -fexceptions -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.cpp, $@)))

$(OBJSA_APP) $(ZOBJSA):
	@mkdir -p $(@D)
	$(AS) -o $@ -c $(ASFLAGS) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.S, $@)))

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
	-$(RM) $(PROJECT)_app.stm32 $(PROJECT)_app.bin $(PROJECT)_boot.bin fsbl.stm32 app.hex boot.hex  

version:
	$(CC) --version
	
# 
# Include the dependency files, should be the last of the makefile
#
-include $(call rwildcard, $(OBJDIR), *.d)

# *** EOF ***
