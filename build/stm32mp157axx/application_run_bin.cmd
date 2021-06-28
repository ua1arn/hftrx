REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
dfu-util.exe --alt 2 --transfer-size 4096 --dfuse-address 0xC0000000:force --download "tc1_stm32mp157axx_app.stm32"
dfu-util.exe --alt 2 --detach
