@rem CONFIG_MACH_SUN8xxxx

SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=..\..\..\arch\aw_f133\device.h
SET FILELIST=risc_cfg.csv risc_wdg.csv risc_timestamp.csv ccu.csv plic.csv sysmap.csv sys_cfg.csv ^
	can.csv uart.csv gpio.csv smhc.csv i2s_pcm.csv dmic.csv owa.csv audio_codec.csv twi.csv spi.csv ^
	cir_rx.csv cir_tx.csv ledc.csv tpadc.csv gpadc.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv ^
	usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv ^
	de3.csv di.csv g2d.csv dsi.csv display_if_top.csv tcon_lcd.csv tcon_tv.csv tve_top.csv csi.csv tvd.csv tv.csv ve.csv ^
	ddrphyc.csv sid.csv dmac.csv pwm.csv ^
	emac.csv clint.csv mctl_com.csv mctl_phy.csv r_ccu.csv r_cpucfg.csv c0_cpux_cfg.csv c0_cpux_mbist.csv ^
	memortmap_t113x.csv

%CONVERTER% --riscv %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > aw_f133.svd

@pause
