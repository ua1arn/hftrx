REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del "tc1_awt507_app_ram.bin"
dfu-util-static --alt 0 --dfuse-address 0x40000000 --upload "tc1_awt507_app_ram.bin"
@pause


