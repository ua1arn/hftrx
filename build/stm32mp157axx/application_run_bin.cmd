REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
dfu-util --alt 2 --transfer-size 4096 --dfuse-address 0xC0000000 --download "tc1_stm32mp157axx_app.stm32"
dfu-util --alt 2 --detach
