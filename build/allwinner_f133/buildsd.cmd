busybox dd oflag=seek_bytes of=sdimage_f133.img bs=1b seek=0 if=bootsector0.bin
busybox dd oflag=seek_bytes of=sdimage_f133.img bs=1b seek=8k if=fsbl.f133
busybox dd oflag=seek_bytes of=sdimage_f133.img bs=1b seek=256k if=tc1_aw_f133_app.f133
pause
