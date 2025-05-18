ifeq (1, 0)
# OpenVG
# see usage of preprocessor HG_FLAT_INCLUDES and EGL_STATIC_LIBRARY symbol
OPENVGDIR = $(PRJROOT)/lib/OpenVG
DINCDIR += \
	$(OPENVGDIR)/include/VG \
	$(OPENVGDIR)/include \
	$(OPENVGDIR)/include/EGL \
	$(OPENVGDIR)/src
SRCDIRS += $(OPENVGDIR)/src
UDEFS += -D"EGL_STATIC_LIBRARY"=1
endif
