@rem CONFIG_MACH_SUN8xxxx

SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\rk3566\device.h
SET FILELIST=gic.csv ^
	uart.csv twi.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > rk3566.svd

@pause
