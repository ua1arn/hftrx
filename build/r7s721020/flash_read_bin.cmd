REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del tc1_r7s721020_app_old.nec32
dfu-util --device 0xFFFF --alt 0 --dfuse-address 0x18020000 --upload "tc1_r7s721020_app_old.nec32"
@pause


