SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_a64\cmsis_a64.h
SET FILELIST= gic.csv cpusubsysctrl.csv ccu.csv sys_cfg.csv ndfc.csv tsc.csv uart.csv gpio.csv gpioint.csv gpioblock.csv smhc.csv i2s_pcm.csv dmic.csv owa.csv audio_codec.csv twi.csv spi.csv cir_rx.csv tpadc.csv gpadc.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv usbehci.csv usbotg.csv usbphyc.csv de2_top.csv de2_bld.csv de2_glb.csv de2_ui.csv de2_vi.csv de2_csr.csv di.csv gpu.csv dsi.csv tcon0.csv tcon1.csv tve_top.csv tve.csv tvd_top.csv tvd.csv c0_cpux_cfg.csv ddrphyc.csv msi_memc.csv sid.csv usb_ehci_capability.csv usb_ohci_capability.csv spinlock.csv dmac.csv pwm.csv emac.csv msgbox.csv smc.csv spc.csv mipi_dsi.csv mipi_dsi_phy.csv hdmi.csv dramcom.csv dramctl0.csv dramphy0.csv dramc.csv sramc.csv r_prcm.csv r_timer.csv r_watchdog.csv 

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_a64.svd

@pause
