REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write BOOTLOADER section of FLASH
@pause
dfu-util --alt 1 --dfuse-address 0x18000000 --download "tc1_r7s721020_boot0.bin"
@pause

