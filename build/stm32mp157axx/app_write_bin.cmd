REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write APPLICATION section of FLASH
@pause
dfu-util.exe --device ffff:0738 --alt 0 --dfuse-address 0x70040000 --download tc1_stm32mp157axx_app.stm32
@pause

