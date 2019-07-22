REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write BOOTLOADER section of FLASH
@pause
dfu-util-static.exe --device ffff:0738 --alt 1 --dfuse-address 0x18000000 --download tc1_r7s721020_boot.bin
@pause

