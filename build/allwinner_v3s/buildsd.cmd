busybox dd oflag=seek_bytes of=sdimage_t113.img bs=1b seek=0 if=bootsector0.bin
busybox dd oflag=seek_bytes of=sdimage_t113.img bs=1b seek=8k if=fsbl.alw32
busybox dd oflag=seek_bytes of=sdimage_t113.img bs=1b seek=256k tc1_t113s3_app.alw32 
pause
