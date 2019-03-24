# $Id$

# List C source files here
SRC += \
 preprocess.c fftwrap.c smallft.c mdf.c filterbank.c kiss_fft.c kiss_fftr.c \
 bandfilters.c \
 board.c \
 usbd.c usbd_desc.c \
 sequen.c \
 elkey.c \
 encoder.c \
 serial.c \
 hardware.c \
 hd44780.c \
 display.c display2.c \
 keyboard.c\
 keymaps.c\
 nvram.c \
 spifuncs.c \
 formats.c \
 synthcalcs.c filters.c \
 pcf8535.c \
 uc1608.c \
 uc1601s.c \
 lph88.c \
 ls020.c \
 l2f50.c \
 s1d13781.c \
 ILI9320.c \
 ili9225.c \
 st7735.c \
 st7565s.c \
 graphltdc.c sdram.c ili8961.c \
 twi.c \
 pio.c \
 tlv320aic23.c cs4272.c nau8822.c wm8994.c \
 hardwarecodecs.c buffers.c audio.c fft.c spislave.c modems.c \
 sdcard.c \
 diskio.c ff.c ffsystem.c ffunicode.c \
 tc1.c tests.c

# List ASM source files here
ASRC +=

# List all user directories here
UINCDIR += ..

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS += 
