ifeq (1, 1)
# ARM CMSIS
CMSISDIR = $(PRJROOT)/CMSIS_6/CMSIS/Core
DINCDIR += $(CMSISDIR)/Include $(CMSISDIR)/Source
UDEFS += -D"CMSIS_device_header"=\"hardware.h\"
endif
