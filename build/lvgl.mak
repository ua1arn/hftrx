ifeq (1, 0)
# Light and Versatile Graphics Library
# https://lvgl.io/
# https://github.com/lvgl/lvgl.git
LVGLDIR = $(PRJROOT)/lvgl
DINCDIR += $(LVGLDIR)/src
SRCDIRS += $(LVGLDIR)/src
#SRCDIRS += $(LVGLDIR)/demos/widgets
endif
