@rem CONFIG_MACH_SUN8xxxx

SET CONVERTER=..\Debug\genregdefs.exe --guid "{E88EF34D-BB95-4cfd-9438-7C6E40DDDD90}"
SET OUTFILE=..\..\..\arch\aw_t113x\device.h
SET FILELIST=gic.csv r_cpucfg.csv r_ccu.csv cpusubsysctrl.csv risc_cfg.csv ccu.csv sys_cfg.csv ^
	uart.csv gpio.csv smhc.csv i2s_pcm.csv dmic.csv owa.csv audio_codec.csv twi.csv spi.csv ^
	cir_rx.csv cir_tx.csv ledc.csv tpadc.csv gpadc.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv can.csv ^
	usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv ^
	de3.csv di.csv g2d.csv dsi.csv hdmi.csv display_if_top.csv tcon_lcd.csv tcon_tv.csv tve.csv csi.csv tvd.csv ^
	c0_cpux_cfg.csv ddrphyc.csv sid.csv ^
	spinlock.csv dmac.csv pwm.csv emac.csv msgbox.csv smc.csv spc.csv mctl_com.csv mctl_phy.csv ^
	tzma.csv dsp_cfg.csv dsp_wdg.csv dsp_intc.csv dsp_tzma.csv dsp_msgbox.csv dcu.csv ve.csv ^
	memortmap_t113x.csv
	

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > aw_t113x.svd

@pause
