@echo Press any key for write APPLICATION and BOOTLOADER
@pause
xfel_fixed_t113-s3 spinor write 0x00000000 fsbl.f133
xfel_fixed_t113-s3 spinor write 0x00040000 tc1_aw_f133_app.f133
@pause
