busybox dd oflag=seek_bytes of=sdimagea733.img bs=1b seek=0 if=bootsector0.bin
busybox dd oflag=seek_bytes of=sdimagea733.img bs=1b seek=1024k if=tc1_awa733_app.alw32
busybox dd oflag=seek_bytes of=sdimagea733.img bs=1b seek=128k if=fsbl.bt0
pause
