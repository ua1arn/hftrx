REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del fsbl_old.stm32
dfu-util --alt 1 --dfuse-address 0x70000000 --upload "fsbl_old.stm32"
@pause


