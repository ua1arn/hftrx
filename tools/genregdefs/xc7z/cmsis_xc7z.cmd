SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=cmsis_xc7z.h
SET LISTOFFILES= ^
	dmacthreads.csv dmacchannels.csv dmac.csv

%CONVERTER% %LISTOFFILES% > %OUTFILE%

@pause
