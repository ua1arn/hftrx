@echo Press any key for write FLASH
@pause
dfu-util-static.exe --device ffff:0738 --alt 0 --dfuse-address 0x18020000 --download tc1_r7s721020_app.bin
@pause

