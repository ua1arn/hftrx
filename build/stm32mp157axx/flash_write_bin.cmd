REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write APPLICATION section of FLASH
@pause
dfu-util --device 0xFFFF --alt 0 --dfuse-address 0x70080000 --download "tc1_stm32mp157axx_app.stm32"
@pause

