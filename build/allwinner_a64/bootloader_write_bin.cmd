REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write FIRST BOOTLOADER section of FLASH
@pause
dfu-util-static --alt 1 --dfuse-address 0x40000000 --download "tc1_t113s3_boot.bin"
@pause

