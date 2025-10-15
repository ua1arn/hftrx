@rem CONFIG_MACH_SUN50IW9

SET CONVERTER=..\Debug\genregdefs.exe --cortexa5x --guid "{9E291677-B6C7-4f4a-BBFD-D0F40B363F83}"
SET OUTFILE=..\..\..\arch\aw_a133\device.h
SET FILELIST= ^
	c0_cpux_cfg.csv ccu.csv cpusubsysctrl.csv ^
	gic.csv gpio.csv ^
	i2s_pcm.csv mipi_dsi.csv ^
	prcm.csv timer.csv ^
	twi.csv uart.csv ^
	usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv ^
	aw_a133.csv
	
%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_a133.svd

@pause
