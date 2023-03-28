# $Id$
#
# program_flash and bootgen are placed in D:\Xilinx\Vitis\2021.2\bin
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

PRJROOT := ../..
OBJDIR = obj

ZINQ_BOOT_DIR := E:
XILINX_JTAGOPTS := -flash_type qspi-x4-single -fsbl fsbl_Y7Z010.elf

#
CC   = $(TRGT)gcc
GPP   = $(TRGT)g++
CP   = $(TRGT)objcopy
AR   = $(TRGT)ar rcs --plugin ../libexec/gcc/$(TRGT2)/12.2.1/liblto_plugin.dll
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex -R .noinit -R .bss -R .vram -R .heap
BIN  = $(CP) -O binary -R .noinit -R .bss -R .vram -R .heap

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
# only -Og for app
# Application
#OPT = -Og
OPT = -Ofast -flto

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

#UADEFS += -D"WITHSMPSYSTEM"

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1

ARCHDIR = $(PRJROOT)/arch/zynq

ZYNQLIBSRC = $(PRJROOT)/lib/zynq/src_7010
ZYNQLIBSRC += $(LWIPDIR)/src $(LWIPDIR)/ports/xilinx 

# full qualified name
ZYNQLIB = zynq7010.a


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
	$(CMSISDSPDIR)/PrivateInclude \
	$(LWIPDIR)/src/include \
	$(LWIPDIR)/src/include/ipv4 \
	$(LWIPDIR)/src/apps/http \
	$(LWIPDIR)/ports/xilinx/include \
	$(OPENVGDIR)/include/VG \
	$(OPENVGDIR)/include \
	$(OPENVGDIR)/include/EGL \
	$(OPENVGDIR)/src \
	$(ZYNQLIBSRC)

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

SRCDIRS = $(PRJROOT)/src
SRCDIRS += $(ARCHDIR)
#SRCDIRS += $(MDLWHOSTDIR)
#SRCDIRS += $(MDLWUSBDIR)
#SRCDIRS += $(LWIPDIR)/src $(PRJROOT)/lib/dhcp-server $(PRJROOT)/lib/dns-server
#SRCDIRS += $(DSP3DDIR)/src
#SRCDIRS += $(OPENVGDIR)/src
#SRCDIRS += $(LWIPDIR)/src $(LWIPDIR)/ports/xilinx 

ifeq (1, 0)
# lvgl
SRCDIRS += $(PRJROOT)/lvgl/src
DINCDIR += $(PRJROOT)/lvgl/src
endif

ifeq (1, 1)
# LuPng
SRCDIRS += $(LUPNGDIR)
DINCDIR += $(LUPNGDIR)
endif

# Define project name here
PROJECT = tc1_xc7z010

# Define linker script file here
LDSCRIPT_BOOT = ./xc7z010xx_ca9_boot.ld
LDSCRIPT_APP = ./xc7z010xx_ca9_app.ld

# List all user C define here, like -D_DEBUG=1
UDEFS += -D"NDEBUG"=1 -D"CPUSTYLE_XC7Z"=1 -D"XC7Z010"=1 -D"EGL_STATIC_LIBRARY"=1
#UDEFS += -D"USE_HAL_DRIVER"=1 -D"USE_FULL_LL_DRIVER"=1

# Define ASM defines here
UADEFS += 

# List C source files here
CSRC +=

# List C++ source files here
CPPSRC +=

# List all user directories here
UINCDIR += $(PRJROOT) $(PRJROOT)/inc

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += $(ZYNQLIB)

VPATH = $(PRJROOT)

#
# End of user defines
##############################################################################################


INCDIR  = $(patsubst %,-I%, $(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%, $(DLIBDIR) $(ULIBDIR))
DEFS    = $(DDEFS) $(UDEFS)
ADEFS   = $(DADEFS) $(UADEFS)


# taken from
# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard = $(foreach d, $(wildcard $(1:=/*)), $(call rwildcard, $(d), $(2)) $(filter $(subst *, %, $(2)), $(d)))

find_c = $(wildcard $(dir)*.c)
find_cpp = $(wildcard $(dir)*.cpp)
find_S = $(wildcard $(dir)*.S)

# Collect files for main module

CSRC += $(call rwildcard, $(SRCDIRS), *.c)
CPPSRC += $(call rwildcard, $(SRCDIRS), *.cpp)
# List ASM source files here
ASRC_APP += $(PRJROOT)/src/crt_CortexA_CPUn.S
ASRC_BOOT += $(PRJROOT)/src/crt_CortexA.S
ASRC_APP += $(ASRC_BOOT)

OBJSC 	+= $(patsubst $(PRJROOT)/%.c, $(OBJDIR)/%.o, $(CSRC))
OBJSCPP 	+= $(patsubst $(PRJROOT)/%.cpp, $(OBJDIR)/%.o, $(CPPSRC))
OBJSA_APP 	+= $(patsubst $(PRJROOT)/%.S, $(OBJDIR)/%.o, $(ASRC_APP))
OBJSA_BOOT 	+= $(patsubst $(PRJROOT)/%.S, $(OBJDIR)/%.o, $(ASRC_BOOT))

SRCS = $(CSRC) $(CPPSRC) $(ASRC)
OBJS = $(OBJSC) $(OBJSCPP)

# Collect files for library

# .S and .s files ugnored
ZCSRC += $(call rwildcard, $(ZYNQLIBSRC), *.c)
ZCPPSRC += $(call rwildcard, $(ZYNQLIBSRC), *.cpp)
ZASRC += $(call rwildcard, $(ZYNQLIBSRC), *.S)

ZOBJSC 	+= $(patsubst $(PRJROOT)/%.c, $(OBJDIR)/%.o, $(ZCSRC))
ZOBJSCPP 	+= $(patsubst $(PRJROOT)/%.cpp, $(OBJDIR)/%.o, $(ZCPPSRC))
ZOBJSA 	+= $(patsubst $(PRJROOT)/%.S, $(OBJDIR)/%.o, $(ZASRC))

ZSRCS = $(ZCSRC) $(ZCPPSRC) $(ZASRC)
ZOBJS = $(ZOBJSC) $(ZOBJSCPP) $(ZOBJSA)

LIBS    = $(DLIBS) $(ULIBS)

MCFLAGS = $(CORE) -fno-math-errno -funroll-loops -fgraphite-identity -ffunction-sections -fdata-sections -ffat-lto-objects -ftree-vectorize
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

.PHONY: all bootloader flash clean lib

all: bitstreams $(PROJECT)_app_xyl32.bin
	-sh -c "cp $(PROJECT)_app_xyl32.bin $(ZINQ_BOOT_DIR)/$(PROJECT)_app_xyl32.bin"

bitstreams: bitstream_4205_10.h bitstream_ant_10.h

bitstream_ant_10.h: bitstream_ant_10.bit
	./bit2c bitstream_ant_10.bit bitstream_ant_10.h

bitstream_4205_10.h: bitstream_4205_10.bit
	./bit2c bitstream_4205_10.bit bitstream_4205_10.h
	
bootloader: bitstreams BOOT2
  
app_run: $(PROJECT)_app.elf bitstream_4205_10.bit app_run_4205.tcl
	xsct.bat app_run_4205.tcl 
  
#app_run: $(PROJECT)_app.elf bitstream_ant_10.bit app_run_ant10.tcl
#	xsct.bat app_run_ant10.tcl  

lib: $(ZYNQLIB)

$(ZYNQLIB): $(ZOBJSC) $(ZOBJSCPP) $(ZOBJSA) 
	-$(RM) $@
	$(AR) $@ $(ZOBJS)
	
flash_dfu: $(PROJECT)_app_xyl32.bin
	./dfu-util --device ffff:0738 --alt 0 --dfuse-address 0xFC080000 --download $(PROJECT)_app_xyl32.bin

flash: $(PROJECT)_app_xyl32.bin
	program_flash -f $(PROJECT)_app_xyl32.bin -offset 0x80000 $(XILINX_JTAGOPTS)

flash2: $(PROJECT)_app_xyl32.bin
	program_flash -f $(PROJECT)_app_xyl32.bin -offset 0x80000 $(XILINX_JTAGOPTS)

$(PROJECT)_app_xyl32.bin: $(PROJECT)_app.bin
	./stm32image -l 0x00104100 -e 0x00104100 -s $(PROJECT)_app.bin -d $@
		
BOOT2: version boot.bin
	-sh -c "cp boot.bin $(ZINQ_BOOT_DIR)/boot.bin"
	
BOOT3: version boot.bin
	program_flash -f BOOT.bin -verify -offset 0x00000 $(XILINX_JTAGOPTS)
	
boot.bin: boot_ant.bif $(PROJECT)_boot.elf
	bootgen -arch zynq -image boot_ant.bif -w on -o $@

$(OBJSC) $(ZOBJSC):
	@mkdir -p $(@D)
	$(CC) -o $@ -std=gnu99 -Wstrict-prototypes -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.c, $@)))

$(OBJSCPP) $(ZOBJSCPP):
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

#	-$(RM) -R $(OBJDIR)

clean: version
	-$(RM) $(OBJS) $(addsuffix .d, $(OBJS))
	-$(RM) $(PROJECT)_boot.elf $(PROJECT)_boot.map
	-$(RM) $(PROJECT)_app.elf $(PROJECT)_app.map
	-$(RM) bitstream_ant_10.h bitstream_ant_20.h
	-$(RM) bitstream_4205_10.h
	-$(RM) bitstream_myir7020.h

version:
	$(CC) --version
	
# 
# Include the dependency files, should be the last of the makefile
#
-include $(call rwildcard, $(OBJDIR), *.d)

# *** EOF ***
