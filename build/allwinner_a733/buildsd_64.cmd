busybox dd oflag=seek_bytes of=sdimagea733_64.img bs=1b seek=0 if=bootsector0.bin
busybox dd oflag=seek_bytes of=sdimagea733_64.img bs=1b seek=8k if=fsbl.bt0
busybox dd oflag=seek_bytes of=sdimagea733_64.img bs=1b seek=256k if=tc1_awa733_64_app.alw32
pause
