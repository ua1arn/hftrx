@rem CONFIG_MACH_SUN50IW9

SET CONVERTER=..\Debug\genregdefs.exe --guid "{9E291677-B6C7-4f4a-BBFD-D0F40B363F83}"
SET OUTFILE=..\..\..\arch\aw_a133\device.h
SET FILELIST= ^
	ccu.csv sid.csv prcm.csv gic.csv gpio.csv ^
	usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv ^
	emac.csv gpu.csv ^
	smc.csv spc.csv ts0.csv nand.csv ^
	uart.csv r_can.csv spi.csv twi.csv smhc.csv scr.csv timer.csv pwm.csv ^
	de33.csv g2d.csv dmac.csv syscfg.csv iommu.csv ^
	ve.csv ce.csv disp_if_top.csv tconlcd.csv hdmi.csv tcon_tv.csv ^
	tve.csv ^
	audio_codec.csv i2s_pcm.csv tsc.csv dmic.csv gpadc.csv lradc.csv owa.csv ths.csv ^
	c0_cpux_cfg.csv cpusubsysctrl.csv r_xxx.csv rtc.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_a133.svd

@pause
