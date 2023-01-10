SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=cmsis_t113s3.h

@echo /* Generated section start */ > %OUTFILE%

%CONVERTER% gic.csv cpusubsysctrl.csv ccu.csv sys_cfg.csv uart.csv gpio.csv gpioint.csv gpioblock.csv smhc.csv i2s_pcm.csv dmic.csv owa.csv audio_codec.csv twi.csv spi.csv cir_rx.csv cir_tx.csv ledc.csv tpadc.csv gpadc.csv spi_dbi.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv can.csv usbehci.csv usbotg_fifo.csv usbotg.csv usbphyc.csv de.csv di.csv g2d_top.csv g2d_mixer.csv g2d_lay.csv g2d_ui.csv g2d_vsu.csv g2d_bld.csv g2d_wb.csv g2d_rot.csv dsi.csv display_top.csv tcon_lcd.csv tve_top.csv tve.csv csic_ccu.csv csic_top.csv csic_parser.csv csic_dma.csv tvd_adc.csv tvd_top.csv tvd.csv c0_cpux_cfg.csv ddrphyc.csv msi_memc.csv sid.csv usb_ehci_capability.csv spinlock.csv dmacchannels.csv dmac.csv pwmchannels.csv pwm.csv emac_addr.csv emac.csv msgbox.csv >> %OUTFILE%

@echo /* Generated section end */ >> %OUTFILE%

@pause
