REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write BOOTLOADER section of FLASH
@pause
dfu-util.exe --device ffff:073c --alt 0 --dfuse-address 0x50000000 --download "tc1_r7s721020_boot.bin"
dfu-util.exe --device ffff:073c --alt 0 --dfuse-address 0x50020000 --download "tc1_r7s721020_app.nec32"
@pause

