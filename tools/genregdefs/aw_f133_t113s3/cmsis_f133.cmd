SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=..\..\..\arch\aw_f133\cmsis_f133.h
SET FILELIST=risc_cfg.csv risc_wdg.csv risc_timestamp.csv ccu.csv plic.csv sys_cfg.csv uart.csv gpio.csv smhc.csv i2s_pcm.csv dmic.csv owa.csv audio_codec.csv twi.csv spi.csv cir_rx.csv cir_tx.csv ledc.csv tpadc.csv gpadc.csv spi_dbi.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv can.csv usbehci.csv usbotg.csv usbphyc.csv de.csv di.csv g2d.csv dsi.csv dsi_dphy.csv display_if_top.csv tcon_lcd.csv tcon_tv.csv tve_top.csv tve.csv csic_ccu.csv csic_top.csv csic_parser.csv csic_dma.csv tvd_top.csv tvd.csv ddrphyc.csv msi_memc.csv sid.csv usb_ehci_capability.csv usb_ohci_capability.csv dmac.csv pwm.csv emac.csv clint.csv mctl_com.csv mctl_phy.csv r_prcm.csv c0_cpux_cfg.csv c0_cpux_mbist.csv

%CONVERTER% --riscv %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > aw_f133.svd

@pause
