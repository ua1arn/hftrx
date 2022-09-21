SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=cmsis_xc7z.h

@echo /* Generated section start */ > %OUTFILE%

%CONVERTER% dmacthreads.csv DMAC_THREAD >> %OUTFILE%
%CONVERTER% dmacchannels.csv DMAC_CH >> %OUTFILE%
%CONVERTER% dmac.csv DMAC >> %OUTFILE%

@echo /* Generated section end */ >> %OUTFILE%

@pause
