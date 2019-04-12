pause "Press any key for write FLASH"
dfu-util-static.exe --device ffff:0738 --dfuse-address 0x18020000 --download tc1_r7s721020_app.bin
pause

