REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write FIRST BOOTLOADER section of FLASH
@pause
dfu-util-static --device 0xFFFF --alt 1 --dfuse-address 0x30002000 --download "fsbl.bt0"
@pause

