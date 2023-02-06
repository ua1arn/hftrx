REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del tc1_t113s3_boot_old.bin
dfu-util-static --alt 1 --dfuse-address 0x40000000 --upload "tc1_t113s3_boot_old.bin"
@pause


