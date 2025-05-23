ifeq (1, 0)
# lvgl
LVGLDIR = $(PRJROOT)/lvgl
DINCDIR += $(LVGLDIR)/src
SRCDIRS += $(LVGLDIR)/src
SRCDIRS += $(LVGLDIR)/demos/widgets
endif
