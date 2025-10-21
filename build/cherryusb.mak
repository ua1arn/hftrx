ifeq (1, 0)
# tinyusb
# https://github.com/cherry-embedded/CherryUSB.git
SRCDIRS += $(PRJROOT)/lib/CherryUSB/common
SRCDIRS += $(PRJROOT)/lib/CherryUSB/core
SRCDIRS += $(PRJROOT)/lib/CherryUSB/class/hub
SRCDIRS += $(PRJROOT)/lib/CherryUSB/class/msc
#
DINCDIR += $(PRJROOT)/lib/CherryUSB/common
DINCDIR += $(PRJROOT)/lib/CherryUSB/core
DINCDIR += $(PRJROOT)/lib/CherryUSB/class/hub
DINCDIR += $(PRJROOT)/lib/CherryUSB/class/msc
endif
