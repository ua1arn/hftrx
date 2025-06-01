xfel_fixed_t113-s3 write 0x00028000 fsbl_frz_s4.alw32
xfel_fixed_t113-s3 exec 0x0002dcb0
busybox sleep 0.5s
xfel_fixed_t113-s3 write 0x40000000 tc1_t113s3_64_app.alw32
xfel_fixed_t113-s3 exec 0x000362d0
pause

