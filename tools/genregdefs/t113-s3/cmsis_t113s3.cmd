SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=cmsis_t113s3.h

@echo /* Generated section start */ > %OUTFILE%
%CONVERTER% cpusubsysctrl.txt CPU_SUBSYS_CTRL >> %OUTFILE%
%CONVERTER% ccu.txt CCU >> %OUTFILE%
%CONVERTER% sys_cfg.txt SYS_CFG >> %OUTFILE%
%CONVERTER% uart.txt UART >> %OUTFILE%
%CONVERTER% gpio.txt GPIO >> %OUTFILE%
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
%CONVERTER% ce_s.txt CE_S >> %OUTFILE%
%CONVERTER% rtc.txt RTC >> %OUTFILE%
%CONVERTER% iommu.txt IOMMU >> %OUTFILE%
%CONVERTER% ths.txt THS >> %OUTFILE%
%CONVERTER% timer.txt TIMER >> %OUTFILE%
%CONVERTER% hstimer.txt HSTIMER >> %OUTFILE%
%CONVERTER% can.txt CAN >> %OUTFILE%
%CONVERTER% usb1.txt USB1 >> %OUTFILE%
%CONVERTER% drd.txt DRD >> %OUTFILE%


%CONVERTER% csi.txt			CSI			 >> %OUTFILE%
%CONVERTER% tvd.txt		TVD		 >> %OUTFILE%

%CONVERTER% de.txt 			DE 		 >> %OUTFILE%	
%CONVERTER% di.txt 			DI 		 >> %OUTFILE%	
%CONVERTER% g2d_top.txt 	G2D_TOP		 >> %OUTFILE%
%CONVERTER% g2d.txt 		G2D 		 >> %OUTFILE%
%CONVERTER% dsi.txt 		DSI 		 >> %OUTFILE%
%CONVERTER% display_top.txt DISPLAY_TOP >> %OUTFILE%
%CONVERTER% tcon_lcd.txt 	TCON_LCD 	 >> %OUTFILE%
%CONVERTER% tve_top.txt 	TVE_TOP 	 >> %OUTFILE%
%CONVERTER% tve.txt 	TVE 	 >> %OUTFILE%

rem %CONVERTER% tv_full.txt TV_FILL 	 >> %OUTFILE%
rem %CONVERTER% tcon_tv.txt		TCON_TV 	 >> %OUTFILE%

%CONVERTER% c0_cpux_cfg.txt 	 C0_CPUX_CFG 	 >> %OUTFILE%
rem %CONVERTER% c0_cpux_mbist.txt C0_CPUX_MBIST 	 >> %OUTFILE%

%CONVERTER% usb_ehci_capability.txt 	 USB_EHCI_Capability 	 >> %OUTFILE%


@echo /* Generated section end */ >> %OUTFILE%
