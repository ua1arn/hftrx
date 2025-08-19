ifeq (1, 0)
# Light and Versatile Graphics Library
# https://lvgl.io/
# https://github.com/lvgl/lvgl.git
LVGLDIR = $(PRJROOT)/lvgl
DINCDIR += $(LVGLDIR)/src
SRCDIRS += $(LVGLDIR)/src
#ASRC_APP += $(call rwildcard, $(LVGLDIR)/src/draw/sw/blend/neon, *.S)
#SRCDIRS += $(LVGLDIR)/demos/widgets
endif
