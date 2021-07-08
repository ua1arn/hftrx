REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
dfu-util --alt 2 --transfer-size 4096 --dfuse-address 0x20000000 --download "tc1_r7s721020_app.nec32"
dfu-util --alt 2 --detach
