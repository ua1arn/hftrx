REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write APPLICATION section of FLASH
@pause
dfu-util-static --alt 0 --dfuse-address 0x40080000 --download "tc1_t113s3_app.alw32"
@pause

