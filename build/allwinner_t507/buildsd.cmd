busybox dd oflag=seek_bytes of=sdimaget507.img bs=1b seek=0 if=bootsector0.bin
busybox dd oflag=seek_bytes of=sdimaget507.img bs=1b seek=8k if=fsbl.alw32
busybox dd oflag=seek_bytes of=sdimaget507.img bs=1b seek=256k if=tc1_awt507_app.alw32
pause
