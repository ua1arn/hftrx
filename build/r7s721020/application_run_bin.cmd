REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
dfu-util-static.exe --device ffff:0750 --alt 2 --dfuse-address 0x20000000 --download tc1_r7s721020_app.bin
dfu-util-static.exe --device ffff:0750 --alt 2 --detach
