@rem CONFIG_MACH_SUN50IW9

SET CONVERTER=..\Debug\genregdefs.exe --cortexa5x "core64_ca.h" --guid "{2A4CD94A-A755-4786-BB96-E52021B2959C}"
SET OUTFILE=..\..\..\arch\aw_a733\device.h
SET FILELIST= ^
	timer.csv gic600.csv gpio.csv twi.csv spi.csv uart.csv ^
	dmac.csv ^
	i2s_pcm.csv ^
	..\aw_generic\usb_ehci_capability.csv ..\aw_generic\usb_ohci_capability.csv ..\aw_generic\usbehci.csv ..\aw_generic\usbotg.csv ^
	ccu.csv rtc.csv prcm.csv sid.csv cpu_subsys.csv intctrl.csv ^
	tcon_tv.csv tconlcd.csv hdmi.csv ^
	display0_top.csv display1_top.csv ^
	aw_a733.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_a733.svd
rem %CONVERTER% --debug aw_a733.csv gic600.csv > gic600.h

@pause
