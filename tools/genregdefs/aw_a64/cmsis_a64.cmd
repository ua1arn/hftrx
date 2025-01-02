SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_a64\device.h
SET FILELIST= ve.csv gic.csv cpusubsysctrl.csv ccu.csv sys_cfg.csv ndfc.csv tsc.csv uart.csv gpio.csv smhc.csv i2s_pcm.csv dmic.csv owa.csv audio_codec.csv twi.csv spi.csv cir_rx.csv tpadc.csv gpadc.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv ^
	usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv ^
	de2.csv di.csv gpu.csv dsi.csv tcon0.csv tcon1.csv ^
	cpux_cfg.csv cpux_mbist.csv ddrphyc.csv sid.csv spinlock.csv dmac.csv pwm.csv emac.csv msgbox.csv smc.csv spc.csv mipi_dsi.csv mipi_dsi_phy.csv hdmi.csv dramcom.csv dramctl0.csv dramphy0.csv dramc.csv sramc.csv r_timer.csv	r_intc.csv r_wdog.csv r_prcm.csv r_twd.csv r_cpucfg.csv r_cir-rx.csv r_pio.csv r_rsb.csv r_pwm.csv 

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_a64.svd

@pause
