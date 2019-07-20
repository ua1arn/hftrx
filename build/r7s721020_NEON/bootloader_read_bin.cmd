REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del tc1_r7s721020_boot_old.bin
dfu-util.exe --device ffff:0738 --alt 1 --dfuse-address 0x18000000 --upload tc1_r7s721020_boot_old.bin
@pause


