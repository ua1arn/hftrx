ifeq (1, 0)
# LWIP
LWIPDIR = $(PRJROOT)/lib/lwip
DINCDIR += \
	$(LWIPDIR)/src/include \
	$(LWIPDIR)/src/apps/http
SRCDIRS += \
	$(LWIPDIR)/src/api \
	$(LWIPDIR)/src/core \
	$(LWIPDIR)/src/netif \
	$(LWIPDIR)/src/http \
	$(LWIPDIR)/src/apps/http \
	$(PRJROOT)/lib/dhcp-server $(PRJROOT)/lib/dns-server
endif
