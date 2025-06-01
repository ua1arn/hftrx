ifeq (1, 0)
# FreeRTOS
RTOSDIR = $(PRJROOT)/lib/FreeRTOS-Kernel-main
DINCDIR += \
	$(RTOSDIR)/include \
	$(RTOSDIR)/portable/GCC/ARM_CA9
SRCDIRS += $(RTOSDIR)
ASRC_APP += $(call rwildcard, $(RTOSDIR)/portable/GCC/ARM_CA9, *.S)
endif
