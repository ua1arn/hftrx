busybox dd oflag=seek_bytes of=sdimage_a64.img bs=1b seek=0 if=bootsector0.bin
busybox dd oflag=seek_bytes of=sdimage_a64.img bs=1b seek=8k if=fsbl.bt0
busybox dd oflag=seek_bytes of=sdimage_a64.img bs=1b seek=128k if=fsbl.bt0
busybox dd oflag=seek_bytes of=sdimage_a64.img bs=1b seek=256k if=tc1_a64_app.alw32
pause
