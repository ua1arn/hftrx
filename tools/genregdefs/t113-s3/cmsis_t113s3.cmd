SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=cmsis_t113s3.h

@echo /* Generated section start */ > %OUTFILE%
%CONVERTER% cpusubsysctrl.txt >> %OUTFILE%
%CONVERTER% ccu.txt >> %OUTFILE%
%CONVERTER% sys_cfg.txt >> %OUTFILE%
%CONVERTER% uart.txt >> %OUTFILE%
%CONVERTER% gpio.txt >> %OUTFILE%
%CONVERTER% gpioint.txt >> %OUTFILE%
%CONVERTER% gpioblock.txt >> %OUTFILE%
%CONVERTER% smhc.txt >> %OUTFILE%
%CONVERTER% i2s_pcm.txt >> %OUTFILE%
%CONVERTER% dmic.txt >> %OUTFILE%
%CONVERTER% owa.txt >> %OUTFILE%
%CONVERTER% audio_codec.txt >> %OUTFILE%
%CONVERTER% twi.txt >> %OUTFILE%
%CONVERTER% spi.txt >> %OUTFILE%
%CONVERTER% cir_rx.txt >> %OUTFILE%
%CONVERTER% cir_tx.txt >> %OUTFILE%
%CONVERTER% ledc.txt >> %OUTFILE%
%CONVERTER% tpadc.txt >> %OUTFILE%
%CONVERTER% gpadc.txt >> %OUTFILE%
%CONVERTER% spi_dbi.txt >> %OUTFILE%
%CONVERTER% ce.txt >> %OUTFILE%
%CONVERTER% rtc.txt >> %OUTFILE%
%CONVERTER% iommu.txt >> %OUTFILE%
%CONVERTER% ths.txt >> %OUTFILE%
%CONVERTER% timer.txt >> %OUTFILE%
%CONVERTER% hstimer.txt >> %OUTFILE%
%CONVERTER% can.txt CAN >> %OUTFILE%
%CONVERTER% usbehci.txt >> %OUTFILE%
%CONVERTER% usbotg_fifo.txt >> %OUTFILE%
%CONVERTER% usbotg.txt >> %OUTFILE%
%CONVERTER% usbphyc.txt >> %OUTFILE%

%CONVERTER% de.txt >> %OUTFILE% 
%CONVERTER% di.txt >> %OUTFILE% 

%CONVERTER% g2d_top.txt >> %OUTFILE%
%CONVERTER% g2d_mixer.txt >> %OUTFILE%
%CONVERTER% g2d_lay.txt >> %OUTFILE%
%CONVERTER% g2d_ui.txt >> %OUTFILE%
%CONVERTER% g2d_vsu.txt >> %OUTFILE%
%CONVERTER% g2d_bld.txt >> %OUTFILE%
%CONVERTER% g2d_wb.txt >> %OUTFILE%
%CONVERTER% g2d_rot.txt >> %OUTFILE%
rem %CONVERTER% g2d.txt >> %OUTFILE%

%CONVERTER% dsi.txt >> %OUTFILE%
%CONVERTER% display_top.txt >> %OUTFILE%
%CONVERTER% tcon_lcd.txt >> %OUTFILE%
%CONVERTER% tve_top.txt >> %OUTFILE%
%CONVERTER% tve.txt >> %OUTFILE%

%CONVERTER% csic_ccu.txt >> %OUTFILE%
%CONVERTER% csic_top.txt >> %OUTFILE%
%CONVERTER% csic_parser.txt >> %OUTFILE%
%CONVERTER% csic_dma.txt >> %OUTFILE%

%CONVERTER% tvd_adc.txt >> %OUTFILE%
%CONVERTER% tvd_top.txt >> %OUTFILE%
%CONVERTER% tvd.txt >> %OUTFILE%

rem %CONVERTER% tv_full.txt >> %OUTFILE%
rem %CONVERTER% tcon_tv.txt >> %OUTFILE%

%CONVERTER% c0_cpux_cfg.txt >> %OUTFILE%
rem %CONVERTER% c0_cpux_mbist.txt >> %OUTFILE%
%CONVERTER% ddrphyc.txt >> %OUTFILE%
%CONVERTER% msi_memc.txt >> %OUTFILE%
%CONVERTER% sid.txt >> %OUTFILE%

%CONVERTER% usb_ehci_capability.txt >> %OUTFILE%

%CONVERTER% spinlock.txt >> %OUTFILE%

%CONVERTER% dmacchannels.txt >> %OUTFILE%
%CONVERTER% dmac.txt >> %OUTFILE%

%CONVERTER% pwmchannels.txt >> %OUTFILE%
%CONVERTER% pwm.txt >> %OUTFILE%

%CONVERTER% emac_addr.txt >> %OUTFILE%
%CONVERTER% emac.txt >> %OUTFILE%

@echo /* Generated section end */ >> %OUTFILE%

@pause
