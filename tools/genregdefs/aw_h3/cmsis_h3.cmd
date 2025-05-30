SET CONVERTER=..\Debug\genregdefs.exe --guid "{BC86EE98-AEBD-4406-AEA7-7519DA69A190}"
SET OUTFILE=..\..\..\arch\aw_h3\device.h
SET FILELIST= ^
	ccu.csv gic.csv cpucfg.csv sysctl.csv ^
	r_prcm.csv rtc.csv ^
	uart.csv gpio.csv timer.csv ^
	de2.csv tcon.csv hdmi.csv ^
	usb_ehci_capability.csv usb_ohci_capability.csv usbotg.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_h3.svd

@pause
