REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del tc1_r7s721020_rom_old.bin
dfu-util-static.exe --device ffff:0738 --alt 0 --dfuse-address 0x18020000 --upload tc1_r7s721020_rom_old.bin
@pause


