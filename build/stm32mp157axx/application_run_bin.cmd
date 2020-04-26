REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
dfu-util.exe --device ffff:0750 --alt 2 --dfuse-address 0xC0000000 --download "tc1_stm32mp157axx_app.stm32"
dfu-util.exe --device ffff:0750 --alt 2 --detach
