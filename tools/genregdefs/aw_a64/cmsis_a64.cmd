SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_a64\cmsis_a64.h

@echo /* Generated section start */ > %OUTFILE%

%CONVERTER% gic.csv cpusubsysctrl.csv ccu.csv sys_cfg.csv ndfc.csv uart.csv gpio.csv gpioint.csv gpioblock.csv smhc.csv i2s_pcm.csv dmic.csv owa.csv audio_codec.csv twi.csv spi.csv cir_rx.csv cir_tx.csv tpadc.csv gpadc.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv usbehci.csv usbotg.csv usbphyc.csv de.csv de_glb.csv de_clk.csv de_bld.csv de_vi.csv de_ui.csv di.csv gpu.csv dsi.csv display_top.csv tcon0.csv tcon1.csv tve_top.csv tve.csv csic_ccu.csv csic_top.csv csic_parser.csv csic_dma.csv tvd_top.csv tvd.csv c0_cpux_cfg.csv ddrphyc.csv msi_memc.csv sid.csv usb_ehci_capability.csv usb_ohci_capability.csv spinlock.csv dmac.csv pwm.csv emac.csv msgbox.csv smc.csv spc.csv mipi_dsi.csv mipi_dsi_phy.csv hdmi.csv >> %OUTFILE%

@echo /* Generated section end */ >> %OUTFILE%

@pause
