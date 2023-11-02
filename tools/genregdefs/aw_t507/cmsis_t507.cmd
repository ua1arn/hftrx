SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_t507\device.h
SET FILELIST= ^
	ccu.csv gic.csv gpio.csv gpu.csv twi.csv ^
	uart.csv usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv ^
	spi.csv timer.csv c0_cpux_cfg.csv cpusubsysctrl.csv pwm.csv smhc.csv ^
	de3.csv g2d.csv tconlcd.csv dmac.csv syscfg.csv iommu.csv ths.csv ce.csv hdmi_tx.csv tcon_tv.csv ^
	audio_codec.csv ahub.csv 

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_t507.svd

@pause
