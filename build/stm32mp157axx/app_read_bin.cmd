REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del app_old.bin
dfu-util.exe --device ffff:0738 --alt 0 --dfuse-address 0x70020000 --upload app_old.bin
@pause


