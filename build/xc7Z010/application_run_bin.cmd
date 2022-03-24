REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
dfu-util.exe --device ffff:0750 --alt 2 --transfer-size 4096 --dfuse-address 0xC0000000 --download "tc1_xc7z010_app_xyl32.bin"
dfu-util.exe --device ffff:0750 --alt 2 --detach
