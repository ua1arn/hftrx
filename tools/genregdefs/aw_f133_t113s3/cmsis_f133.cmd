SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=..\..\..\arch\aw_f133\cmsis_f133.h
SET FILELIST=risc_cfg.csv risc_wdg.csv risc_timestamp.csv ccu_f133a.csv plic.csv sys_cfg.csv uart.csv gpio.csv gpioint.csv gpioblock.csv smhc.csv i2s_pcm.csv dmic.csv owa.csv audio_codec.csv twi.csv spi.csv cir_rx.csv cir_tx.csv ledc.csv tpadc.csv gpadc.csv spi_dbi.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv can.csv usbehci.csv usbotg.csv usbphyc.csv de.csv de_glb.csv de_top.csv de_bld.csv de_vi.csv de_ui.csv di.csv g2d_top.csv g2d_mixer.csv g2d_vi.csv g2d_ui.csv g2d_vsu.csv g2d_bld.csv g2d_wb.csv g2d_rot.csv dsi.csv display_if_top.csv tcon_lcd.csv tcon_tv.csv tve_top.csv tve.csv csic_ccu.csv csic_top.csv csic_parser.csv csic_dma.csv tvd_top.csv tvd.csv ddrphyc.csv msi_memc.csv sid.csv usb_ehci_capability.csv usb_ohci_capability.csv dmac.csv pwm.csv emac.csv clint.csv mctl_com.csv mctl_phy.csv

%CONVERTER% --riscv %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > aw_f133.svd

@pause
