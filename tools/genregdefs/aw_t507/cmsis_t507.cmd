SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_t507\cmsis_t507.h
SET FILELIST= ccu.csv gic.csv gpio.csv gpu.csv twi.csv uart.csv usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv g2d.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_t507.svd

@pause
