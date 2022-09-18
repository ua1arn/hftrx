SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=cmsis_t113s3.h

@echo /* Generated section start */ > %OUTFILE%
%CONVERTER% cpusubsysctrl.txt >> %OUTFILE%
%CONVERTER% ccu.txt >> %OUTFILE%
%CONVERTER% sys_cfg.txt SYS_CFG >> %OUTFILE%
%CONVERTER% uart.txt UART >> %OUTFILE%
%CONVERTER% gpio.txt GPIO >> %OUTFILE%
%CONVERTER% gpioint.txt GPIOINT >> %OUTFILE%
%CONVERTER% gpioblock.txt GPIOBLOCK >> %OUTFILE%
%CONVERTER% smhc.txt SMHC >> %OUTFILE%
%CONVERTER% i2s_pcm.txt I2S_PCM >> %OUTFILE%
%CONVERTER% dmic.txt DMIC >> %OUTFILE%
%CONVERTER% owa.txt OWA >> %OUTFILE%
%CONVERTER% audio_codec.txt AUDIO_CODEC >> %OUTFILE%
%CONVERTER% twi.txt TWI >> %OUTFILE%
%CONVERTER% spi.txt SPI >> %OUTFILE%
%CONVERTER% cir_rx.txt CIR_RX >> %OUTFILE%
%CONVERTER% cir_tx.txt CIR_TX >> %OUTFILE%
%CONVERTER% ledc.txt LEDC >> %OUTFILE%
%CONVERTER% tpadc.txt TPADC >> %OUTFILE%
%CONVERTER% gpadc.txt GPADC >> %OUTFILE%
%CONVERTER% spi_dbi.txt SPI_DBI >> %OUTFILE%
%CONVERTER% ce.txt CE >> %OUTFILE%
%CONVERTER% rtc.txt RTC >> %OUTFILE%
%CONVERTER% iommu.txt IOMMU >> %OUTFILE%
%CONVERTER% ths.txt THS >> %OUTFILE%
%CONVERTER% timer.txt TIMER >> %OUTFILE%
%CONVERTER% hstimer.txt HSTIMER >> %OUTFILE%
%CONVERTER% can.txt CAN >> %OUTFILE%
%CONVERTER% usbehci.txt USBEHCI >> %OUTFILE%
%CONVERTER% usbotg_fifo.txt USBOTGFIFO >> %OUTFILE%
%CONVERTER% usbotg.txt USBOTG >> %OUTFILE%
%CONVERTER% usbphyc.txt USBPHYC >> %OUTFILE%

%CONVERTER% de.txt DE >> %OUTFILE% 
%CONVERTER% di.txt DI >> %OUTFILE% 

%CONVERTER% g2d_top.txt G2D_TOP >> %OUTFILE%
%CONVERTER% g2d_mixer.txt G2D_MIXER >> %OUTFILE%
%CONVERTER% g2d_lay.txt G2D_LAY >> %OUTFILE%
%CONVERTER% g2d_ui.txt G2D_UI >> %OUTFILE%
%CONVERTER% g2d_vsu.txt G2D_VSU >> %OUTFILE%
%CONVERTER% g2d_bld.txt G2D_BLD >> %OUTFILE%
%CONVERTER% g2d_wb.txt G2D_WB >> %OUTFILE%
%CONVERTER% g2d_rot.txt G2D_ROT >> %OUTFILE%
rem %CONVERTER% g2d.txt G2D >> %OUTFILE%

%CONVERTER% dsi.txt DSI >> %OUTFILE%
%CONVERTER% display_top.txt DISPLAY_TOP >> %OUTFILE%
%CONVERTER% tcon_lcd.txt TCON_LCD >> %OUTFILE%
%CONVERTER% tve_top.txt TVE_TOP >> %OUTFILE%
%CONVERTER% tve.txt TVE >> %OUTFILE%

%CONVERTER% csic_ccu.txt CSIC_CCU >> %OUTFILE%
%CONVERTER% csic_top.txt CSIC_TOP >> %OUTFILE%
%CONVERTER% csic_parser.txt CSIC_PARSER >> %OUTFILE%
%CONVERTER% csic_dma.txt CSIC_DMA >> %OUTFILE%

%CONVERTER% tvd_adc.txt TVD_ADC >> %OUTFILE%
%CONVERTER% tvd_top.txt TVD_TOP >> %OUTFILE%
%CONVERTER% tvd.txt TVD >> %OUTFILE%

rem %CONVERTER% tv_full.txt TV_FILL >> %OUTFILE%
rem %CONVERTER% tcon_tv.txt TCON_TV >> %OUTFILE%

%CONVERTER% c0_cpux_cfg.txt C0_CPUX_CFG >> %OUTFILE%
rem %CONVERTER% c0_cpux_mbist.txt C0_CPUX_MBIST >> %OUTFILE%
%CONVERTER% ddrphyc.txt >> %OUTFILE%
%CONVERTER% msi_memc.txt MSI_MEMC >> %OUTFILE%
%CONVERTER% sid.txt SID >> %OUTFILE%

%CONVERTER% usb_ehci_capability.txt USB_EHCI_Capability >> %OUTFILE%

%CONVERTER% spinlock.txt SPINLOCK >> %OUTFILE%

%CONVERTER% dmacchannels.txt DMAC_CH >> %OUTFILE%
%CONVERTER% dmac.txt DMAC >> %OUTFILE%

%CONVERTER% pwmchannels.txt PWM_CH >> %OUTFILE%
%CONVERTER% pwm.txt PWM >> %OUTFILE%

%CONVERTER% emac_addr.txt EMAC_ADDR >> %OUTFILE%
%CONVERTER% emac.txt EMAC >> %OUTFILE%

@echo /* Generated section end */ >> %OUTFILE%

@pause
