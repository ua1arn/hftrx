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

#UADEFS += -D"WITHSMPSYSTEM"

PRJROOT := ../..
OBJDIR = obj

# Define project name here
PROJECT = tc1_alwnrt113s3

# Define linker script file here
LDSCRIPT_BOOT = ./allwnr_t113s3_boot.ld
LDSCRIPT_APP = ./allwnr_t113s3_app.ld
ARCHDIR = $(PRJROOT)/arch/allwnr_t13s3

# List all user C define here, like -D_DEBUG=1
UDEFS += -D"NDEBUG"=1 -D"CPUSTYPE_ALLWNT113"=1 -D"EGL_STATIC_LIBRARY"=1
#UDEFS += -D"USE_HAL_DRIVER"=1 -D"USE_FULL_LL_DRIVER"=1
#
CC   = $(TRGT)gcc
GPP   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex -R .noinit -R .bss -R .vram -R .heap
BIN  = $(CP) -O binary -R .noinit -R .bss -R .vram -R .heap

# This product incorporates the ARM dual-core Cortex-A7 MPCore, where the IP version is r0p5
# debug architecture ARMv7.1.
CORE = -mcpu=cortex-a7
CORE += -mfloat-abi=hard
#CORE += -mfpu=neon
#CORE += -mfpu=neon-vfpv4
CORE += -mfpu=vfpv4

# Define optimisation level here
# olny -Og for app
# Application
#OPT = -Og
OPT = -Ofast -flto

CMSISDIR = $(PRJROOT)/CMSIS_5/CMSIS
LWIPDIR = $(PRJROOT)/lib/lwip-2.1.2
# see usage of preprocessor HG_FLAT_INCLUDES and EGL_STATIC_LIBRARY symbol
OPENVGDIR = $(PRJROOT)/lib/OpenVG
DSP3DDIR = $(PRJROOT)/lib/dsp3D
USBXDIR = $(PRJROOT)/src/hal
MDLWUSBDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Device_Library
MDLWHOSTDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Host_Library
#HALDIR = $(PRJROOT)/lib/Drivers/STM32MP1xx_HAL_Driver

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
	$(OPENVGDIR)/src

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
#SRCDIRS += $(DSP3DDIR)/src
#SRCDIRS += $(LWIPDIR)/src $(PRJROOT)/lib/dhcp-server $(PRJROOT)/lib/dns-server
#SRCDIRS += $(OPENVGDIR)/src
SRCDIRS += $(ARCHDIR)
#SRCDIRS += $(HALDIR) $(MDLWUSBDIR) $(MDLWHOSTDIR)

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
LDFLAGS_BOOT = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --gc-sections -T$(LDSCRIPT_BOOT) -Wl,-Map=$(PROJECT)_boot.map,--cref $(LIBDIR)
LDFLAGS_APP = $(MCFLAGS) $(OPT) -nostartfiles -Xlinker --gc-sections -T$(LDSCRIPT_APP) -Wl,-Map=$(PROJECT)_app.map,--cref $(LIBDIR)

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

all: APP2 $(PROJECT)_app.alw32 app.hex

allZ:
	@echo Src
	@echo $(abspath $(CSRC))
	@echo Obj at $(OBJDIR)
	@echo $(abspath $(OBJS))

bootloader: BOOT2 fsbl.alw32 boot.hex
  
app_runAAA: fsbl.alw32
	./xfel-windows-v1.2.7/xfel write 0x00028000 fsbl.alw32
	./xfel-windows-v1.2.7/xfel exec 0x00028000 
  
app_run: pboot.bin $(PROJECT)_app.bin
	./xfel-windows-v1.2.7/xfel ddr t113-s3
	./xfel-windows-v1.2.7/xfel write 0x00028000 pboot.bin
	./xfel-windows-v1.2.7/xfel write 0x40000000 $(PROJECT)_app.bin
	./xfel-windows-v1.2.7/xfel exec 0x00028000 

flash: $(PROJECT)_app.alw32
	./dfu-util --alt 0 --dfuse-address 0x70080000 --download $(PROJECT)_app.alw32

BOOT2: version $(PROJECT)_boot.elf $(PROJECT)_boot.bin

APP2: version $(PROJECT)_app.elf $(PROJECT)_app.bin

fsbl.alw32: $(PROJECT)_boot.bin
	cp boottemplate.bin fsbl.alw32.tmp
	./wtitefsbl.exe fsbl.alw32.tmp 0x00100 tc1_alwnrt113s3_boot.bin -w
	./checksum.exe fsbl.alw32.tmp fsbl.alw32
	
boot.hex: fsbl.alw32
	./bin2ihex -l 0x00000000 -s fsbl.alw32 >$@
	
$(PROJECT)_app.alw32: $(PROJECT)_app.bin
	./stm32image -l 0xC0000100 -e 0xC0000100 -s $(PROJECT)_app.bin -d $@
	
app.hex: $(PROJECT)_app.alw32
	./bin2ihex -l 0x00080000 -s $(PROJECT)_app.alw32 >$@

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
