SET CONVERTER=..\Debug\genregdefs.exe 
SET OUTFILE=..\..\..\arch\vm14\device.h
SET FILELIST=pmctr.csv smctr.csv cmctr.csv gic.csv uart.csv gpio.csv sdma.csv nandmport.csv normport.csv mfbsp.csv i2c.csv i2s.csv swic.csv swic_mfpsp_dma.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > elvees_vm14.svd

@pause
