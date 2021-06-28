REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write SECOND BOOTLOADER section of FLASH
@pause
dfu-util --alt 1 --dfuse-address 0x70040000 --download "fsbl.stm32"
@pause

