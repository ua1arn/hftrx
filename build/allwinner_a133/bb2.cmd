@rem bootable image should be linked to 0x00020000 base address
@rem http://nskhuman.ru/allwinner/card/cardmode.php?ysclid=lkq00i1n2061987481
@set TARGET=_DISKLETTER

bootutil tc1_awt507_app.bin 0x40000100 monitor.bin

@rem cfimager -raw -offset 0x2000 -f boot0_sdcard_sun50iw9p1.bin -d %TARGET%
cfimager -raw -offset 0x2000 -f curboot.bin -d %TARGET%
@rem cfimager -raw -offset 0x1004000 -f monitor.bin -d %TARGET%
@rem cfimager -raw -offset 0x1004000 -f tt.txt -d %TARGET%
