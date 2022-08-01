SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=cmsis_xc7z.h

@echo /* Generated section start */ > %OUTFILE%

%CONVERTER% dmacthreads.txt DMAC_THREAD >> %OUTFILE%
%CONVERTER% dmacchannels.txt DMAC_CH >> %OUTFILE%
%CONVERTER% dmac.txt DMAC >> %OUTFILE%

@echo /* Generated section end */ >> %OUTFILE%

@pause
