SET CONVERTER=..\Debug\genregdefs.exe --guid "{BC86EE98-AEBD-4406-AEA7-7519DA69A190}"
SET OUTFILE=..\..\..\arch\aw_h3\device.h
SET FILELIST= ^
	ccu.csv gic.csv cpucfg.csv sysctl.csv ^
	r_prcm.csv rtc.csv ^
	uart.csv gpio.csv timer.csv ^
	de2.csv tcon.csv hdmi.csv ^
	..\aw_generic\usbehci.csv ..\aw_generic\usb_ehci_capability.csv ..\aw_generic\usb_ohci_capability.csv ..\aw_generic\usbotg.csv ^
	aw_h3.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_h3.svd

@pause
