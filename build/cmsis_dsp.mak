ifeq (1, 1)
# CMSIS_DSP
# https://github.com/ARM-software/CMSIS-DSP.git
CMSISDSPDIR=$(PRJROOT)/CMSIS-DSP
#
SRCDIRS +=
DINCDIR += \
	$(CMSISDSPDIR)/Include \
	$(CMSISDSPDIR)/Ne10 \
	$(CMSISDSPDIR)/ComputeLibrary/Include \
	$(CMSISDSPDIR)/Source \
	$(CMSISDSPDIR)/PrivateInclude
endif
