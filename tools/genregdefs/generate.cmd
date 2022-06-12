echo /* Generated section start */ > tt.txt
.\Debug\genregdefs.exe cpusubsysctrl.txt CPU_SUBSYS_CTRL >> tt.txt
.\Debug\genregdefs.exe ccu.txt CCU >> tt.txt
.\Debug\genregdefs.exe sys_cfg.txt SYS_CFG >> tt.txt
.\Debug\genregdefs.exe uart.txt UART >> tt.txt
.\Debug\genregdefs.exe gpio.txt GPIO >> tt.txt
.\Debug\genregdefs.exe smhc.txt SMHC >> tt.txt
.\Debug\genregdefs.exe i2s_pcm.txt I2S_PCM >> tt.txt
.\Debug\genregdefs.exe dmic.txt DMIC >> tt.txt
.\Debug\genregdefs.exe owa.txt OWA >> tt.txt
.\Debug\genregdefs.exe audio_codec.txt AUDIO_CODEC >> tt.txt
.\Debug\genregdefs.exe twi.txt TWI >> tt.txt
.\Debug\genregdefs.exe spi.txt SPI >> tt.txt
.\Debug\genregdefs.exe cir_rx.txt CIR_RX >> tt.txt
.\Debug\genregdefs.exe cir_tx.txt CIR_TX >> tt.txt
.\Debug\genregdefs.exe ledc.txt LEDC >> tt.txt
.\Debug\genregdefs.exe tpadc.txt TPADC >> tt.txt
.\Debug\genregdefs.exe gpadc.txt GPADC >> tt.txt
.\Debug\genregdefs.exe spi_dbi.txt SPI_DBI >> tt.txt
.\Debug\genregdefs.exe ce_s.txt CE_S >> tt.txt
.\Debug\genregdefs.exe rtc.txt RTC >> tt.txt
.\Debug\genregdefs.exe iommu.txt IOMMU >> tt.txt
.\Debug\genregdefs.exe ths.txt THS >> tt.txt
.\Debug\genregdefs.exe timer.txt TIMER >> tt.txt
.\Debug\genregdefs.exe hstimer.txt HSTIMER >> tt.txt
.\Debug\genregdefs.exe can.txt CAN >> tt.txt
.\Debug\genregdefs.exe usb1.txt USB1 >> tt.txt
.\Debug\genregdefs.exe drd.txt DRD >> tt.txt


.\Debug\genregdefs.exe csi.txt			CSI			 >> tt.txt
.\Debug\genregdefs.exe tvd.txt		TVD		 >> tt.txt

.\Debug\genregdefs.exe de.txt 			DE 		 >> tt.txt	
.\Debug\genregdefs.exe di.txt 			DI 		 >> tt.txt	
.\Debug\genregdefs.exe g2d.txt 		G2D 		 >> tt.txt
.\Debug\genregdefs.exe dsi.txt 		DSI 		 >> tt.txt
.\Debug\genregdefs.exe display_top.txt DISPLAY_TOP  >> tt.txt
.\Debug\genregdefs.exe tcon_lcd.txt 	TCON_LCD 	 >> tt.txt
.\Debug\genregdefs.exe tve.txt 	TVE 	 >> tt.txt

rem .\Debug\genregdefs.exe tv_full.txt TV_FILL 	 >> tt.txt
rem .\Debug\genregdefs.exe tcon_tv.txt		TCON_TV 	 >> tt.txt

.\Debug\genregdefs.exe c0_cpux_cfg.txt 	 C0_CPUX_CFG 	 >> tt.txt
rem .\Debug\genregdefs.exe c0_cpux_mbist.txt C0_CPUX_MBIST 	 >> tt.txt

.\Debug\genregdefs.exe usb_ehci_capability.txt 	 USB_EHCI_Capability 	 >> tt.txt


echo /* Generated section end */ >> tt.txt
