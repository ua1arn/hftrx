@rem CONFIG_MACH_SUN8xxxx

SET CONVERTER=..\Debug\genregdefs.exe --guid "{25B09AFF-0510-4ff4-AC97-A98D3B4BAC38}"
SET OUTFILE=..\..\..\arch\aw_v3s\device.h
SET FILELIST=gic.csv ccu.csv rtc.csv ^
	uart.csv gpio.csv twi.csv spi.csv ^
	timer.csv pwm.csv dmac.csv ^
	ve.csv ^
	usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv ^
	dram.csv sys_cfg.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > aw_v3s.svd

@pause
