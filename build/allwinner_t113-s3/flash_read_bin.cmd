REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
del tc1_t113s3_app_old.alw32
dfu-util-static --alt 0 --dfuse-address 0x40080000 --upload "tc1_t113s3_app_old.alw32"
@pause


