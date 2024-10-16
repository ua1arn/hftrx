SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_h3\device.h
SET FILELIST= ^
	ccu.csv gic.csv ^
	uart.csv gpio.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_h3.svd

@pause
