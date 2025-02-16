REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write FLASH
@pause
dfu-util --device 0xFFFF --alt 0 --dfuse-address 0x18020000 --download "tc1_r7s721020_app.nec32"
@pause

