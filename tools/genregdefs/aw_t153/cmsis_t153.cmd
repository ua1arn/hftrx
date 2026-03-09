@rem CONFIG_MACH_SUN8xxxx

SET CONVERTER=..\Debug\genregdefs.exe --guid "{858514A9-1AE9-4033-9FAD-BC80BB4264BB}"
SET OUTFILE=..\..\..\arch\aw_t153\device.h
SET FILELIST= ^
	timer.csv gic600.csv gpio.csv twi.csv spi.csv uart.csv smhc.csv ^
	dmac.csv ^
	i2s_pcm.csv ^
	..\aw_generic\usb_ehci_capability.csv ..\aw_generic\usb_ohci_capability.csv ..\aw_generic\usbehci.csv ..\aw_generic\usbotg.csv ^
	ccu.csv rtc.csv prcm.csv sid.csv cpu_subsys.csv intctrl.csv ^
	g2d.csv de33.csv tcon_tv.csv tconlcd.csv hdmi.csv ^
	display0_top.csv display1_top.csv ^
	aw_t153.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_t153.svd
rem %CONVERTER% --debug aw_t153.csv gic600.csv > gic600.h

@pause
