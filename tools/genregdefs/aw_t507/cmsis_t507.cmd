SET CONVERTER=..\Debug\genregdefs.exe
SET OUTFILE=..\..\..\arch\aw_t507\cmsis_t507.h
SET FILELIST= ccu.csv gic.csv gpio.csv gpu.csv twi.csv uart.csv usbehci.csv usbotg.csv usbphyc.csv usb_ehci_capability.csv usb_ohci_capability.csv g2d.csv spi.csv timer.csv c0_cpux_cfg.csv cpusubsysctrl.csv pwm.csv smhc.csv de3.csv tconlcd.csv dmac.csv ahub.csv syscfg.csv iommu.csv ths.csv ce.csv hdmi_tx.csv

%CONVERTER% %FILELIST% > %OUTFILE%

%CONVERTER% --svd %FILELIST% > aw_t507.svd

@pause
