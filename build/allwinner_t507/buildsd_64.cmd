busybox dd oflag=seek_bytes of=sdimaget507_64.img bs=1b seek=0 if=bootsector0.bin
busybox dd oflag=seek_bytes of=sdimaget507_64.img bs=1b seek=8k if=fsbl.bt0
busybox dd oflag=seek_bytes of=sdimaget507_64.img bs=1b seek=256k if=tc1_awt507_64_app.alw32
pause
