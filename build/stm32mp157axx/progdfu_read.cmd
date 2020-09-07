REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del tc1_r7s721020_boot_old.bin
dfu-util.exe --device ffff:073c --alt 0 --dfuse-address 0x50000000 --upload "tc1_stm32mp157axx_full_old.bin"
@pause

