@echo Press any key for write APPLICATION and BOOTLOADER
@pause
xfel_fixed_t113-s3 spinor write 0x00000000 fsbl.alw32
xfel_fixed_t113-s3 spinor write 0x00040000 tc1_t113s3_app.alw32
@pause
