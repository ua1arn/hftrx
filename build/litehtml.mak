ifeq (1, 0)
# litehtml is the lightweight HTML rendering engine with CSS2/CSS3 support
# http://www.litehtml.com/
LITEHTMLDIR = $(PRJROOT)/lib/litehtml
DINCDIR += $(LITEHTMLDIR)/include $(LITEHTMLDIR)/src/gumbo/include $(LITEHTMLDIR)/include/litehtml $(LITEHTMLDIR)/src/gumbo/include/gumbo
SRCDIRS += $(LITEHTMLDIR)/src
UDEFS += -D"LITEHTML_NO_THREADS"=1
endif
