@echo Press any key for erase APPLICATION and BOOTLOADER
@pause
xfel_fixed_t113-s3 spinor erase 0x00000000 1024
xfel_fixed_t113-s3 spinor erase 0x00040000 1024
@pause
