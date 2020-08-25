REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write BOOTLOADER section of FLASH
@pause
dfu-util.exe --device ffff:073c --alt 0 --dfuse-address 0x50000000 --download "fsbl.stm32"
dfu-util.exe --device ffff:073c --alt 0 --dfuse-address 0x50040000 --download "fsbl.stm32"
dfu-util.exe --device ffff:073c --alt 0 --dfuse-address 0x50080000 --download "tc1_stm32mp157axx_app.stm32"
@pause

