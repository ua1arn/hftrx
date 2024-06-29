@rem CONFIG_MACH_SUN8xxxx

SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_v3s\device.h
SET FILELIST=gic.csv ccu.csv rtc.csv ^
	uart.csv gpio.csv twi.csv ^
	timer.csv pwm.csv dmac.csv ^
	ve.csv ^
	dram.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > aw_v3s.svd

@pause
