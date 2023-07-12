SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_v853x\cmsis_v853x.h
SET FILELIST=gic.csv r_cpucfg.csv cpusubsysctrl.csv risc_cfg.csv ccu.csv sys_cfg.csv uart.csv gpio.csv smhc.csv i2s_pcm.csv dmic.csv audio_codec.csv twi.csv spi.csv cir_rx.csv cir_tx.csv ledc.csv tpadc.csv gpadc.csv ce.csv rtc.csv iommu.csv ths.csv timer.csv hstimer.csv can.csv usbehci.csv usbotg.csv usbphyc.csv de.csv di.csv g2d.csv dsi.csv dsi_dphy.csv display_if_top.csv tcon_lcd.csv tcon_tv.csv tve.csv csi_v853x.csv tvd.csv c0_cpux_cfg.csv ddrphyc.csv msi_memc.csv sid.csv usb_ehci_capability.csv usb_ohci_capability.csv spinlock.csv dmac.csv pwm.csv emac.csv msgbox.csv smc.csv spc.csv mctl_com.csv mctl_phy.csv r_prcm.csv dcu.csv tv.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > aw_v853x.svd

@pause
