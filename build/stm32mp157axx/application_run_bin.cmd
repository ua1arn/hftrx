REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
dfu-util.exe --device ffff:0750 --alt 2 --dfuse-address 0xC0000000 --download stm32mp157axx_app.bin
dfu-util.exe --device ffff:0750 --alt 2 --detach
