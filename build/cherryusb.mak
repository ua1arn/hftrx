ifeq (1, 0)
# CherryUSB
# https://github.com/cherry-embedded/CherryUSB.git
CHERRYUSBDIR=$(PRJROOT)/lib/CherryUSB/
#
SRCDIRS += $(CHERRYUSBDIR)/common
SRCDIRS += $(CHERRYUSBDIR)/core
SRCDIRS += $(CHERRYUSBDIR)/class/hub
SRCDIRS += $(CHERRYUSBDIR)/class/msc
#
DINCDIR += $(CHERRYUSBDIR)/common
DINCDIR += $(CHERRYUSBDIR)/core
DINCDIR += $(CHERRYUSBDIR)/class/hub
DINCDIR += $(CHERRYUSBDIR)/class/msc
endif
