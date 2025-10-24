ifeq (1, 0)
# rt-thread
# https://github.com/RT-Thread/rt-thread.git
RTTHREADDIR=$(PRJROOT)/lib/rt-thread

UDEFS += -D"__RT_KERNEL_SOURCE__"=1

SRCDIRS += $(RTTHREADDIR)/src

ASRC_APP += $(RTTHREADDIR)/libcpu/aarch64/common/mp/context_gcc.S

DINCDIR += $(RTTHREADDIR)/include
DINCDIR += $(RTTHREADDIR)/libcpu/aarch64/common/include
DINCDIR += $(RTTHREADDIR)/components/finsh
DINCDIR += $(RTTHREADDIR)/components/utilities/utest
       
endif
