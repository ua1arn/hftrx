SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=dmac.h

%CONVERTER% dmacchannels.txt DMAC_CH > %OUTFILE%
%CONVERTER% dmac.txt DMAC >> %OUTFILE%
