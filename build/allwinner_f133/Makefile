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
TRGT2 = riscv-none-elf
TRGT = $(TRGT2)-
#TRGT = C:\user\klen\bin\arm-kgp-eabi-

#XFEL = ../../tools/xfel-windows-v1.3.2/xfel
XFEL = ./xfel_fixed_t113-s3

#UADEFS += -D"WITHSMPSYSTEM"

PRJROOT := ../..
OBJDIR = obj

# Define project name here
PROJECT = tc1_aw_f133

# Define linker script file here
LDSCRIPT_BOOT = ./aw_f133_boot.ld
LDSCRIPT_APP = ./aw_f133_app.ld
ARCHDIR = $(PRJROOT)/arch/aw_f133

# List all user C define here, like -D_DEBUG=1
UDEFS += -D"NDEBUG"=1 -D"CPUSTYLE_F133"=1
#UDEFS += -D"USE_HAL_DRIVER"=1 -D"USE_FULL_LL_DRIVER"=1
#
CC   = $(TRGT)gcc
GPP   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary
AR   = $(TRGT)ar rcs --plugin ../libexec/gcc/$(TRGT2)/14.2.1/liblto_plugin.dll

# The D1s features a single RV64GCV core XuanTie C906 from T-Head Semiconductor (subsidiary of Alibaba).

# misa --X-VU-S-----M---I--F-DC-A

#CORE = -march=rv64imafdc_zicsr -mabi=lp64d -mcmodel=medany -fno-stack-protector --specs=nosys.specs
CORE = -march=rv64imafd_zicsr -mabi=lp64d -mcmodel=medany -fno-stack-protector --specs=nosys.specs

# Define optimisation level here
#OPT = -Og
#OPT = -Ofast -flto
#OPT = -Os -flto
OPT = -Ofast

CMSISDIR = $(PRJROOT)/CMSIS_6/CMSIS
CMSISDSPDIR = $(PRJROOT)/CMSIS-DSP
# see usage of preprocessor HG_FLAT_INCLUDES and EGL_STATIC_LIBRARY symbol
USBXDIR = $(PRJROOT)/src/hal
TINYUSBDIR = $(PRJROOT)/lib/hftrx_tinyusb

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1
# List all default directories to look for include files here
DINCDIR = \
	$(ARCHDIR)
	
DINCDIR += \
	$(MDLWUSBDIR)/Core/Inc \
	$(MDLWHOSTDIR)/Core/Inc \
	$(USBXDIR)
	
DINCDIR += \
	$(CMSISDIR)/Core/Include $(CMSISDIR)/Core/Source
	
DINCDIR += \
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
SRCDIRS += $(ARCHDIR)

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
# https://github.com/hathach/tinyusb.git
SRCDIRS += $(PRJROOT)/lib/hftrx_tinyusb/src
DINCDIR += $(PRJROOT)/lib/hftrx_tinyusb/src
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

CSRC += $(call rwildcard, $(SRCDIRS), *.c)
CPPSRC += $(call rwildcard, $(SRCDIRS), *.cpp)

# not all .S files in directory need
#ASRC += $(call rwildcard, $(SRCDIRS), *.S)
#ASRC_APP += $(PRJROOT)/src/crt_CortexA_CPUn.S
ASRC_BOOT += $(PRJROOT)/src/crt_RV64IFDV.S
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
LDFLAGS_BOOT = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --sort-section=alignment -Xlinker --gc-sections -T$(LDSCRIPT_BOOT) -Wl,-Map=$(PROJECT)_boot.map,--cref $(LIBDIR)
LDFLAGS_APP = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --sort-section=alignment -Xlinker --gc-sections -T$(LDSCRIPT_APP) -Wl,-Map=$(PROJECT)_app.map,--cref $(LIBDIR)

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

.PHONY: all bootloader flash clean erase

all: APP2 $(PROJECT)_app.f133

allZ:
	@echo Src
	@echo $(abspath $(CSRC))
	@echo Obj at $(OBJDIR)
	@echo $(abspath $(OBJS))

bootloader: BOOT2 fsbl.f133
  
app_run: $(PROJECT)_app.f133
	$(XFEL) ddr f133
	$(XFEL) write 0x40000000 $(PROJECT)_app.f133
	$(XFEL) exec 0x40000100 
  
# bootloader debug - check aw_f133_boot.ld
_app_run: fsbl.f133
	$(XFEL) write 0x00028000 fsbl.f133
	$(XFEL) exec 0x00028000 

flash:
	-$(XFEL) spinor write 0x00000000 fsbl.f133
	$(XFEL) spinor write 0x00040000 $(PROJECT)_app.f133

erase:
	$(XFEL) spinor erase 0x00000000 1024
	$(XFEL) spinor erase 0x00040000 1024

BOOT2: version $(PROJECT)_boot.elf $(PROJECT)_boot.bin

APP2: version $(PROJECT)_app.elf $(PROJECT)_app.bin

fsbl.f133: $(PROJECT)_boot.bin
	./egontool -rv $(PROJECT)_boot.bin fsbl.f133
	
$(PROJECT)_app.f133: $(PROJECT)_app.bin
	./stm32image -l 0x40000100 -e 0x40000100 -s $(PROJECT)_app.bin -d $@

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
	-$(RM) $(PROJECT)_app.f133 fsbl.f133 $(PROJECT)_app.bin $(PROJECT)_boot.bin 

version:
	$(CC) --version
	
# 
# Include the dependency files, should be the last of the makefile
#
-include $(call rwildcard, $(OBJDIR), *.d)

# *** EOF ***
