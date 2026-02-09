@rem CONFIG_MACH_SUN50IW9

SET CONVERTER=..\Debug\genregdefs.exe --cortexa5x "core64_ca.h" --guid "{2A4CD94A-A755-4786-BB96-E52021B2959C}"
SET OUTFILE=..\..\..\arch\aw_a733\device.h
SET FILELIST= ^
	timer.csv gic600.csv gpio.csv twi.csv spi.csv uart.csv ^
	usb_ehci_capability.csv usb_ohci_capability.csv usbehci.csv usbotg.csv ^
	ccu.csv prcm.csv sid.csv cpu_subsys.csv ^
	aw_a733.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_a733.svd
rem %CONVERTER% --debug tconlcd.csv > aw_a733.txt

@pause
