SET CONVERTER=..\Debug\genregdefs.exe --guid "{193A12F0-6292-4f92-BE20-2DBDD3EB1B8A}"
SET OUTFILE=cmsis_xc7z.h
SET LISTOFFILES= ^
	gic.csv ^
	dmacthreads.csv dmacchannels.csv dmac.csv

%CONVERTER% %LISTOFFILES% > %OUTFILE%

@pause
