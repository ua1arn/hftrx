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
.\Debug\genregdefs.exe USB1.txt USB1 >> tt.txt


echo /* Generated section end */ >> tt.txt
