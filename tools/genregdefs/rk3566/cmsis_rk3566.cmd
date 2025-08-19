@rem CONFIG_MACH_SUN8xxxx

SET CONVERTER=..\Debug\genregdefs.exe --cortexa5x --guid "{2F3EAF83-E6C0-48a6-A374-C924EDE2E572}"
SET OUTFILE=..\..\..\arch\rk3566\device.h
SET FILELIST=gic.csv ^
	uart.csv twi.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --riscv --svd %FILELIST% > rk3566.svd

@pause
