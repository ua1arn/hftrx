SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=cmsis_f133.h

@echo /* Generated section start */ > %OUTFILE%
%CONVERTER% risc_reg.csv >> %OUTFILE%
%CONVERTER% risc_wdg.csv >> %OUTFILE%
%CONVERTER% risc_timestamp.csv >> %OUTFILE%
%CONVERTER% ccu.csv >> %OUTFILE%
%CONVERTER% risc_plic.csv >> %OUTFILE%
%CONVERTER% sys_cfg.csv >> %OUTFILE%
%CONVERTER% uart.csv >> %OUTFILE%
%CONVERTER% gpio.csv >> %OUTFILE%
%CONVERTER% gpioint.csv >> %OUTFILE%
%CONVERTER% gpioblock.csv >> %OUTFILE%
%CONVERTER% smhc.csv >> %OUTFILE%
%CONVERTER% i2s_pcm.csv >> %OUTFILE%
%CONVERTER% dmic.csv >> %OUTFILE%
%CONVERTER% owa.csv >> %OUTFILE%
%CONVERTER% audio_codec.csv >> %OUTFILE%
%CONVERTER% twi.csv >> %OUTFILE%
%CONVERTER% spi.csv >> %OUTFILE%
%CONVERTER% cir_rx.csv >> %OUTFILE%
%CONVERTER% cir_tx.csv >> %OUTFILE%
%CONVERTER% ledc.csv >> %OUTFILE%
%CONVERTER% tpadc.csv >> %OUTFILE%
%CONVERTER% gpadc.csv >> %OUTFILE%
%CONVERTER% spi_dbi.csv >> %OUTFILE%
%CONVERTER% ce.csv >> %OUTFILE%
%CONVERTER% rtc.csv >> %OUTFILE%
%CONVERTER% iommu.csv >> %OUTFILE%
%CONVERTER% ths.csv >> %OUTFILE%
%CONVERTER% timer.csv >> %OUTFILE%
%CONVERTER% hstimer.csv >> %OUTFILE%
%CONVERTER% can.csv CAN >> %OUTFILE%
%CONVERTER% usbehci.csv >> %OUTFILE%
%CONVERTER% usbotg_fifo.csv >> %OUTFILE%
%CONVERTER% usbotg.csv >> %OUTFILE%
%CONVERTER% usbphyc.csv >> %OUTFILE%

%CONVERTER% de.csv >> %OUTFILE% 
%CONVERTER% di.csv >> %OUTFILE% 

%CONVERTER% g2d_top.csv >> %OUTFILE%
%CONVERTER% g2d_mixer.csv >> %OUTFILE%
%CONVERTER% g2d_lay.csv >> %OUTFILE%
%CONVERTER% g2d_ui.csv >> %OUTFILE%
%CONVERTER% g2d_vsu.csv >> %OUTFILE%
%CONVERTER% g2d_bld.csv >> %OUTFILE%
%CONVERTER% g2d_wb.csv >> %OUTFILE%
%CONVERTER% g2d_rot.csv >> %OUTFILE%
rem %CONVERTER% g2d.csv >> %OUTFILE%

%CONVERTER% dsi.csv >> %OUTFILE%
%CONVERTER% display_top.csv >> %OUTFILE%
%CONVERTER% tcon_lcd.csv >> %OUTFILE%
%CONVERTER% tve_top.csv >> %OUTFILE%
%CONVERTER% tve.csv >> %OUTFILE%

%CONVERTER% csic_ccu.csv >> %OUTFILE%
%CONVERTER% csic_top.csv >> %OUTFILE%
%CONVERTER% csic_parser.csv >> %OUTFILE%
%CONVERTER% csic_dma.csv >> %OUTFILE%

%CONVERTER% tvd_adc.csv >> %OUTFILE%
%CONVERTER% tvd_top.csv >> %OUTFILE%
%CONVERTER% tvd.csv >> %OUTFILE%

rem %CONVERTER% tv_full.csv >> %OUTFILE%
rem %CONVERTER% tcon_tv.csv >> %OUTFILE%

%CONVERTER% c0_cpux_cfg.csv >> %OUTFILE%
rem %CONVERTER% c0_cpux_mbist.csv >> %OUTFILE%
%CONVERTER% ddrphyc.csv >> %OUTFILE%
%CONVERTER% msi_memc.csv >> %OUTFILE%
%CONVERTER% sid.csv >> %OUTFILE%

%CONVERTER% usb_ehci_capability.csv >> %OUTFILE%

%CONVERTER% dmacchannels.csv >> %OUTFILE%
%CONVERTER% dmac.csv >> %OUTFILE%

%CONVERTER% pwmchannels.csv >> %OUTFILE%
%CONVERTER% pwm.csv >> %OUTFILE%

%CONVERTER% emac_addr.csv >> %OUTFILE%
%CONVERTER% emac.csv >> %OUTFILE%

@echo /* Generated section end */ >> %OUTFILE%

@pause
