SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=..\..\..\arch\vm14\elvees_vm14_2.h
SET FILELIST=gic.csv uart.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > elvees_vm14.svd

@pause
