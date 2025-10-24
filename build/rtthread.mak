ifeq (1, 0)
# rt-thread
# https://github.com/RT-Thread/rt-thread.git
RTTHREADDIR=$(PRJROOT)/lib/rt-thread
SRCDIRS += $(RTTHREADDIR)/src
DINCDIR += $(RTTHREADDIR)/src
endif
