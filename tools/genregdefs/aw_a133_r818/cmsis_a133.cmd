@rem CONFIG_MACH_SUN50IW9

SET CONVERTER=..\Debug\genregdefs.exe --cortexa5x "core64_ca.h" --guid "{9E291677-B6C7-4f4a-BBFD-D0F40B363F83}"
SET OUTFILE=..\..\..\arch\aw_a133\device.h
SET FILELIST= ^
	c0_cpux_cfg.csv ccu.csv cpusubsysctrl.csv ^
	gic.csv gpio.csv ^
	i2s_pcm.csv mipi_dsi.csv ^
	prcm.csv timer.csv dmac.csv ^
	twi.csv spi.csv uart.csv ^
	..\aw_generic\usbehci.csv ..\aw_generic\usbotg.csv usbphyc.csv ..\aw_generic\usb_ehci_capability.csv ..\aw_generic\usb_ohci_capability.csv ^
	aw_a133.csv
	
%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_a133.svd

@pause
