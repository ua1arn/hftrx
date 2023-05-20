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
#TRGT2 = aarch64-none-elf
TRGT = $(TRGT2)-

PRJROOT := ../..
OBJDIR = obj

ZINQ_BOOT_DIR := E:

#
CC   = $(TRGT)gcc
GPP   = $(TRGT)g++
CP   = $(TRGT)objcopy
AR   = $(TRGT)ar rcs --plugin ../libexec/gcc/$(TRGT2)/12.2.1/liblto_plugin.dll
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size --format=sysv --radix=16
HEX  = $(CP) -O ihex -R .noinit -R .bss -R .vram -R .heap
BIN  = $(CP) -O binary -R .noinit -R .bss -R .vram -R .heap

# Cortex-A53 r0p4-50rel0
# Neon r0p4-00rel0

# The Cortex-A53 processor is a mid-range, low-power processor that implements the Armv8-A architecture

#CORE = -mcpu=cortex-a53 -march=armv8-a
CORE = -mcpu=cortex-a53
#CORE += -mfloat-abi=hard 
#CORE += -mfpu=vfpv4
#CORE += -mfpu=neon
#CORE += -mfpu=neon-vfpv4
#CORE += -mlong-calls

# Define optimisation level here
# only -Og for app
# Application
#OPT = -Og
#OPT = -Ofast -flto
OPT = -Os

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

#UADEFS += -D"WITHSMPSYSTEM"

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = -D__ASSEMBLY__=1

ARCHDIR = $(PRJROOT)/arch/zynqmp

ZYNQLIBSRC = $(PRJROOT)/lib/zynq/src

# full qualified name
#ZYNQLIB = zynq7000.a


CMSISDIR = $(PRJROOT)/CMSIS_5/CMSIS
CMSISDSPDIR = $(PRJROOT)/CMSIS-DSP
LWIPDIR = $(PRJROOT)/lib/lwip-2.1.2
# see usage of preprocessor HG_FLAT_INCLUDES and EGL_STATIC_LIBRARY symbol
OPENVGDIR = $(PRJROOT)/lib/OpenVG
DSP3DDIR = $(PRJROOT)/lib/dsp3D
USBXDIR = $(PRJROOT)/src/hal
MDLWUSBDIR = $(PRJROOT)/lib/Middlewares/ST/STM32_USB_Device_Library
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

#SRCDIRS = $(PRJROOT)/src $(OPENVGDIR)/src $(LWIPDIR)/src $(DSP3DDIR)/src
SRCDIRS = $(PRJROOT)/src
SRCDIRS += $(ARCHDIR)

# Define project name here
PROJECT = tc1_xczu2

# Define linker script file here
LDSCRIPT_BOOT = ./xczuxx_ca53_boot.ld
LDSCRIPT_APP = ./xczuxx_ca53_app.ld

# Ultrascale:
# 	CPUSTYLE_XCZU
# 	XCZU2..XCZU9, XCZU11
#
# List all user C define here, like -D_DEBUG=1
UDEFS += -D"NDEBUG"=1 -D"CPUSTYLE_CA53"=1 -D"CPUSTYLE_XCZU"=1 -D"XCZU2"=1 -D"EGL_STATIC_LIBRARY"=1

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


ifeq (1, 0)
# FreeRTOS
RTOSDIR = $(PRJROOT)/lib/FreeRTOS-Kernel-main
DINCDIR += \
	$(RTOSDIR)/include \
	$(RTOSDIR)/portable/GCC/ARM_CA9
SRCDIRS += $(RTOSDIR)
ASRC_APP += $(call rwildcard, $(RTOSDIR)/portable/GCC/ARM_CA9, *.S)
endif

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
#ASRC += $(PRJROOT)/src/crt_CortexA.S $(PRJROOT)/src/crt_CortexA_CPUn.S

OBJSC 	+= $(patsubst $(PRJROOT)/%.c, $(OBJDIR)/%.o, $(CSRC))
OBJSCPP 	+= $(patsubst $(PRJROOT)/%.cpp, $(OBJDIR)/%.o, $(CPPSRC))
OBJSA 	+= $(patsubst $(PRJROOT)/%.S, $(OBJDIR)/%.o, $(ASRC))

SRCS = $(CSRC) $(CPPSRC) $(ASRC)
OBJS = $(OBJSC) $(OBJSCPP) $(OBJSA)

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

.PHONY: all bootloader flash clean lib

all: bitstream_alinx_axu2cga.h $(PROJECT)_app.xyl32
	-sh -c "cp $(PROJECT)_app.xyl32 $(ZINQ_BOOT_DIR)/$(PROJECT)_app.xyl32"

bitstream_alinx_axu2cga.h: bitstream_alinx_axu2cga.bit
	./bit2c bitstream_alinx_axu2cga.bit bitstream_alinx_axu2cga.h
	
bootloader: BOOT2
  
  
app_run: $(PROJECT)_app.elf bitstream_alinx_axu2cga.bit app_run_axu2cga.tcl
	xsct.bat app_run_axu2cga.tcl  
 

lib: $(ZYNQLIB)

$(ZYNQLIB): $(ZOBJSC) $(ZOBJSCPP) $(ZOBJSA) 
	-$(RM) $@
	$(AR) $@ $(ZOBJS)
	
flash: $(PROJECT)_app.xyl32
	./dfu-util --device ffff:0738 --alt 0 --dfuse-address 0xFC080000 --download $(PROJECT)_app.xyl32

$(PROJECT)_app.xyl32: $(PROJECT)_app.bin
	./stm32image -l 0x00200100 -e 0x00200100 -s $(PROJECT)_app.bin -d $@
		
BOOT2: version boot.bin
	-sh -c "cp boot.bin $(ZINQ_BOOT_DIR)/boot.bin"

boot.bin: boot_alinx_axu2cga.bif $(PROJECT)_boot.elf
	bootgen -arch zynqmp -image boot_alinx_axu2cga.bif -w on -o $@

$(OBJSC) $(ZOBJSC):
	@mkdir -p $(@D)
	$(CC) -o $@ -std=gnu99 -Wstrict-prototypes -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.c, $@)))

$(OBJSCPP) $(ZOBJSCPP):
	@mkdir -p $(@D)
	$(GPP) -o $@ -std=gnu++11 -fno-rtti -fexceptions -c $(CPFLAGS) $(INCDIR) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.cpp, $@)))

$(OBJSA) $(ZOBJSA):
	@mkdir -p $(@D)
	$(AS) -o $@ -c $(ASFLAGS) $(addprefix $(PRJROOT)/, $(patsubst $(OBJDIR)/%, %, $(patsubst %.o, %.S, $@)))

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

#	-$(RM) -R $(OBJDIR)

clean: version
	-$(RM) $(OBJS)
	-$(RM) $(PROJECT)_boot.elf $(PROJECT)_boot.map
	-$(RM) $(PROJECT)_app.elf $(PROJECT)_app.map
	-$(RM) bitstream_alinx_axu2cga.h

version:
	$(CC) --version
	
# 
# Include the dependency files, should be the last of the makefile
#
-include $(call rwildcard, $(OBJDIR), *.d)

# *** EOF ***
