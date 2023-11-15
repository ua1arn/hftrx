SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_t507\device.h
SET FILELIST= ^
	ccu.csv gic.csv gpio.csv gpu.csv twi.csv ^
	usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv ^
	uart.csv spi.csv timer.csv pwm.csv smhc.csv scr.csv ^
	de3.csv g2d.csv tconlcd.csv dmac.csv syscfg.csv iommu.csv ths.csv ce.csv hdmi_tx.csv tcon_tv.csv ^
	audio_codec.csv ahub.csv tsc.csv dmic.csv gpadc.csv lradc.csv owa.csv ^
	c0_cpux_cfg.csv cpusubsysctrl.csv r_xxx.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_t507.svd

@pause
