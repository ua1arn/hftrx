REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del app_old.stm32
dfu-util.exe --device ffff:0738 --alt 1 --dfuse-address 0x70000000 --upload app_old.stm32
@pause


