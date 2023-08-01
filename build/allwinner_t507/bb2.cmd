@rem bootable image should be linked to 0x00020000 base address
@rem http://nskhuman.ru/allwinner/card/cardmode.php?ysclid=lkq00i1n2061987481
@set TARGET=E_but_may_be_changed

rem cfimager -raw -offset 0x2000 -f boot0_sdcard_sun50iw9p1.bin -d %TARGET%
bootutil
cfimager -raw -offset 0x2000 -f curboot.bin -d %TARGET%
cfimager -raw -offset 0x1004000 -f o.bin -d %TARGET%
rem cfimager -raw -offset 0x1004000 -f tt.txt -d %TARGET%
